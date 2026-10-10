/**
 * \file database/metadata_from_database_value.c
 *
 * \brief Create a \ref metadata instance from a \ref database value.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/* forward decls. */
static status verify_mac(
    const database_value* value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* derived_mac_key);
static status decrypt_ciphertext(
    secure_buffer** plaintext, RCPR_SYM(allocator)* alloc,
    const database_value* value, const secure_buffer* derived_enc_key);

/**
 * \brief Create a \ref metadata instance from the given \ref database_value and
 * \p encryption_key.
 *
 * \note This method verifies the message authentication code in this
 * \ref database_value using the secret key.

 * \param meta              Pointer to the \ref metadata pointer to be set
 *                          with the created instance on success.
 * \param alloc             The allocator to use for this operation.
 * \param value             The \ref database_value to use for this operation.
 * \param encryption_key    The encryption key to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
metadata_from_database_value(
    metadata** meta, RCPR_SYM(allocator)* alloc,
    const database_value* value, const secure_buffer* encryption_key)
{
    status retval, release_retval;
    secure_buffer *derived_enc_key, *derived_mac_key, *plaintext;

    /* create the derived encryption key buffer. */
    retval =
        database_derived_key_create(
            &derived_enc_key, alloc, encryption_key, value->IV,
            DATABASE_ENCRYPTION_KEY_OFFSET);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the derived mac key buffer. */
    retval =
        database_derived_key_create(
            &derived_mac_key, alloc, encryption_key, value->IV,
            DATABASE_MAC_KEY_OFFSET);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_derived_enc_key;
    }

    /* verify the value mac. */
    retval = verify_mac(value, alloc, derived_mac_key);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_derived_mac_key;
    }

    /* decrypt the ciphertext to form the plaintext. */
    retval = decrypt_ciphertext(&plaintext, alloc, value, derived_enc_key);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_derived_mac_key;
    }

    /* create the metadata from the plaintext; the result of this operation is
     * the result of this function. */
    retval = metadata_from_buffer(meta, alloc, plaintext);
    goto cleanup_plaintext;

cleanup_plaintext:
    release_retval = resource_release(secure_buffer_resource_handle(plaintext));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_derived_mac_key:
    release_retval =
        resource_release(secure_buffer_resource_handle(derived_mac_key));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_derived_enc_key:
    release_retval =
        resource_release(secure_buffer_resource_handle(derived_enc_key));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Verify the message authentication code for this database value using
 * the provided derived mac key.
 *
 * \param value                 The database value to use for this verification.
 * \param alloc                 The allocator to use for this operation.
 * \param derived_mac_key       The derived mac key to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status verify_mac(
    const database_value* value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* derived_mac_key)
{
    status retval, release_retval;
    const uint8_t *database_key_data, *IV_data, *ciphertext_data, *mac_data,
                  *mac_key_data;
    uint8_t* computed_mac_data;
    size_t database_key_size, IV_size, ciphertext_size, mac_size, mac_key_size,
           computed_mac_size;
    secure_buffer* computed_mac;
    EVP_MD_CTX *mctx;
    EVP_PKEY *mkey;

    /* create a buffer for storing the computed mac. */
    retval = secure_buffer_create(&computed_mac, alloc, SHA384_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    database_key_data =
        secure_buffer_data(
            &database_key_size, (secure_buffer*)value->database_key);
    IV_data = secure_buffer_data(&IV_size, (secure_buffer*)value->IV);
    ciphertext_data =
        secure_buffer_data(&ciphertext_size, (secure_buffer*)value->ciphertext);
    mac_data = secure_buffer_data(&mac_size, (secure_buffer*)value->mac);
    mac_key_data =
        secure_buffer_data(&mac_key_size, (secure_buffer*)derived_mac_key);
    computed_mac_data = secure_buffer_data(&computed_mac_size, computed_mac);

    /* create MAC key. */
    mkey =
        EVP_PKEY_new_mac_key(
            EVP_PKEY_HMAC, NULL, mac_key_data, (int)mac_key_size);
    if (NULL == mkey)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_computed_mac;
    }

    /* create MAC context. */
    mctx = EVP_MD_CTX_new();
    if (NULL == mctx)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mkey;
    }

    /* initialize HMAC. */
    retval = EVP_DigestSignInit(mctx, NULL, EVP_sha384(), NULL, mkey);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* add database key to MAC. */
    retval = EVP_DigestSignUpdate(mctx, database_key_data, database_key_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* add the IV to MAC. */
    retval = EVP_DigestSignUpdate(mctx, IV_data, IV_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* add the ciphertext to MAC. */
    retval = EVP_DigestSignUpdate(mctx, ciphertext_data, ciphertext_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* finalize the MAC. */
    size_t mac_size_out = mac_size;
    retval = EVP_DigestSignFinal(mctx, computed_mac_data, &mac_size_out);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* verify the mac. */
    retval = CRYPTO_memcmp(mac_data, computed_mac_data, mac_size);
    if (0 != retval)
    {
        retval = ERROR_DATABASE_VALUE_BAD_BLOB;
        goto cleanup_mctx;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto cleanup_mctx;

cleanup_mctx:
    EVP_MD_CTX_free(mctx);

cleanup_mkey:
    EVP_PKEY_free(mkey);

cleanup_computed_mac:
    release_retval =
        resource_release(secure_buffer_resource_handle(computed_mac));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Decrypt the ciphertext for a database value using the derived
 * encryption key.
 *
 * \param plaintext             Pointer to the buffer pointer to receive the
 *                              decrypted plaintext on success.
 * \param alloc                 The allocator to use for this operation.
 * \param value                 The database value to use for this operation.
 * \param derived_enc_key       The derived encryption key to use for this
 *                              operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status decrypt_ciphertext(
    secure_buffer** plaintext, RCPR_SYM(allocator)* alloc,
    const database_value* value, const secure_buffer* derived_enc_key)
{
    status retval, release_retval;
    secure_buffer* tmp = NULL;
    const uint8_t *enc_key_data, *ciphertext_data, *IV_data;
    uint8_t *plaintext_data;
    size_t enc_key_size, ciphertext_size, IV_size, plaintext_size;
    EVP_CIPHER_CTX* enc;
    int length;

    /* get the input buffers. */
    enc_key_data =
        secure_buffer_data(&enc_key_size, (secure_buffer*)derived_enc_key);
    ciphertext_data =
        secure_buffer_data(&ciphertext_size, (secure_buffer*)value->ciphertext);
    IV_data = secure_buffer_data(&IV_size, (secure_buffer*)value->IV);

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, ciphertext_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the output buffer data. */
    plaintext_data = secure_buffer_data(&plaintext_size, tmp);

    /* skip to the IV for the cipher. */
    IV_data += 32;

    /* create a cipher context. */
    enc = EVP_CIPHER_CTX_new();
    if (NULL == enc)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_tmp;
    }

    /* initialize the cipher. */
    retval =
        EVP_DecryptInit_ex(
            enc, EVP_aes_256_ctr(), NULL, enc_key_data, IV_data);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* decrypt the ciphertext. */
    retval =
        EVP_DecryptUpdate(
            enc, (void*)plaintext_data, &length, ciphertext_data,
            ciphertext_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* finalize... technically a no-op. */
    retval =
        EVP_DecryptFinal_ex(enc, (void*)(plaintext_data + length), &length);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* success. */
    *plaintext = tmp;
    tmp = NULL;
    retval = STATUS_SUCCESS;
    goto cleanup_enc;

cleanup_enc:
    EVP_CIPHER_CTX_free(enc);

cleanup_tmp:
    if (NULL != tmp)
    {
        release_retval = resource_release(secure_buffer_resource_handle(tmp));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

done:
    return retval;
}
