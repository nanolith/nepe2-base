/**
 * \file database/database_value_create.c
 *
 * \brief Create a \ref database_value instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <rcpr/vtable.h>

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

RCPR_VTABLE resource_vtable database_value_vtable = {
    .release = &database_value_resource_release
};

static status create_database_key(
    database_value* value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session, const metadata* meta);
static status encrypt_plaintext(
    database_value* value, const secure_buffer* plaintext,
    const secure_buffer* encryption_key);
static status mac_database_value(
    database_value* value, const secure_buffer* mac_key);

/**
 * \brief Create a \ref database_value instance from the given verification
 * passphrase, master passphrase, session passphrase, encryption key, and
 * metadata.
 *
 * \note The legacy flag in the metadata controls whether this is a legacy
 * Nepephemeral 1 entry or a modern metadata entry. This, in turn, controls the
 * database key that is generated.
 *
 * \param value             Pointer to the \ref database_value pointer to be set
 *                          with this value on success.
 * \param alloc             The allocator to use for this operation.
 * \param meta              The metadata to use to create this value.
 * \param verify            The verification passphrase for this operation.
 * \param master            The master passphrase for this operation.
 * \param session           The session passphrase for this operation.
 * \param encryption_key    The encryption key for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_value_create(
    database_value** value, RCPR_SYM(allocator)* alloc, const metadata* meta,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session, const secure_buffer* encryption_key)
{
    status retval, release_retval;
    database_value* tmp = NULL;
    secure_buffer *plaintext, *derived_enc_key, *derived_mac_key;
    uint8_t *IV_data;
    size_t IV_size;

    /* allocate memory for this instance. */
    retval = rcpr_allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize resource. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &database_value_vtable);
    tmp->alloc = alloc;

    /* create the database key. */
    retval = create_database_key(tmp, alloc, verify, master, session, meta);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* create a buffer for holding the IV. */
    retval = secure_buffer_create(&tmp->IV, alloc, 48);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* get IV buffer. */
    IV_data = secure_buffer_data(&IV_size, tmp->IV);

    /* Initialize IV with cryptographically random data. */
    retval = RAND_bytes(IV_data, IV_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_IV_GENERATE_FAILED;
        goto cleanup_tmp;
    }

    /* Create the derived encryption key buffer. */
    retval =
        database_derived_key_create(
            &derived_enc_key, alloc, encryption_key, tmp->IV, 0x01020304);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* Create the derived mac key buffer. */
    retval =
        database_derived_key_create(
            &derived_mac_key, alloc, encryption_key, tmp->IV, 0x90807060);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_derived_enc_key;
    }

    /* Create the plaintext data. */
    retval = metadata_to_buffer(&plaintext, alloc, meta);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_derived_mac_key;
    }

    /* encrypt this plaintext data. */
    retval = encrypt_plaintext(tmp, plaintext, derived_enc_key);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_plaintext;
    }

    /* Compute the record MAC. */
    retval = mac_database_value(tmp, derived_mac_key);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_plaintext;
    }

    /* success. */
    *value = tmp;
    tmp = NULL;
    retval = STATUS_SUCCESS;
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

cleanup_tmp:
    if (NULL != tmp)
    {
        release_retval = resource_release(&tmp->hdr);
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

done:
    return retval;
}

/**
 * \brief Create the appropriate database key for this value by examining the
 * metadata. If the legacy flag is set to true, create a legacy key, otherwise,
 * create a regular key.
 *
 * \param value             The value to which this key is stored.
 * \param alloc             The allocator to use for this operation.
 * \param verify            The verification passphrase.
 * \param master            The master passphrase.
 * \param session           The session passphrase.
 * \param metadata          The metadata for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status create_database_key(
    database_value* value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session, const metadata* meta)
{
    status retval;
    bool legacy;

    /* get the legacy flag from the metadata. */
    retval = metadata_legacy_flag_get(&legacy, meta);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* should we generate the legacy key? */
    if (legacy)
    {
        retval =
            database_create_legacy_key(
                &value->database_key, alloc, verify, master, session);
    }
    else
    {
        retval =
            database_create_key(
                &value->database_key, alloc, verify, master, session);
    }

    goto done;

done:
    return retval;
}

/**
 * \brief Encrypt the given plaintext buffer, storing the value in the
 * ciphertext field in the \ref database_value instance.
 *
 * \param value             The instance to which the ciphertext will be stored.
 * \param plaintext         The plaintext buffer to encrypt.
 * \param encryption_key    The derived encryption key to use for this
 *                          operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status encrypt_plaintext(
    database_value* value, const secure_buffer* plaintext,
    const secure_buffer* encryption_key)
{
    status retval;
    const uint8_t *plaintext_data, *ciphertext_data, *encryption_key_data,
                  *IV_data;
    size_t plaintext_size, ciphertext_size, encryption_key_size, IV_size;
    EVP_CIPHER_CTX* enc;
    int length;

    /* get the plaintext data. */
    plaintext_data =
        secure_buffer_data(&plaintext_size, (secure_buffer*)plaintext);

    /* create the ciphertext buffer instance. */
    retval =
        secure_buffer_create(&value->ciphertext, value->alloc, plaintext_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the buffer data. */
    ciphertext_data = secure_buffer_data(&ciphertext_size, value->ciphertext);
    encryption_key_data =
        secure_buffer_data(
            &encryption_key_size, (secure_buffer*)encryption_key);
    IV_data = secure_buffer_data(&IV_size, (secure_buffer*)value->IV);

    /* skip to the IV for the cipher. */
    IV_data += 32;


    /* create a cipher context. */
    enc = EVP_CIPHER_CTX_new();
    if (NULL == enc)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto done;
    }

    /* initialize the cipher. */
    retval =
        EVP_EncryptInit_ex(
            enc, EVP_aes_256_ctr(), NULL, encryption_key_data, IV_data);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* encrypt the plaintext. */
    retval =
        EVP_EncryptUpdate(
            enc, (void*)ciphertext_data, &length, plaintext_data,
            plaintext_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* finalize... technically a no-op. */
    retval =
        EVP_EncryptFinal_ex(enc, (void*)(ciphertext_data + length), &length);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_CIPHER_CREATE;
        goto cleanup_enc;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto cleanup_enc;

cleanup_enc:
    EVP_CIPHER_CTX_free(enc);

done:
    return retval;
}

/**
 * \brief Compute the HMAC on the database value for storing in the database.
 *
 * \param value                 The value to which this MAC will be saved.
 * \param mac_key               The key to use to compute this MAC.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status mac_database_value(
    database_value* value, const secure_buffer* mac_key)
{
    status retval;
    const uint8_t *database_key_data, *IV_data, *ciphertext_data, *mac_key_data;
    uint8_t *mac_data;
    size_t database_key_size, IV_size, ciphertext_size, mac_size, mac_key_size;
    EVP_MD_CTX *mctx;
    EVP_PKEY *mkey;

    /* create a buffer for storing the mac. */
    retval =
        secure_buffer_create(&value->mac, value->alloc, SHA384_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    database_key_data =
        secure_buffer_data(&database_key_size, value->database_key);
    IV_data = secure_buffer_data(&IV_size, value->IV);
    ciphertext_data = secure_buffer_data(&ciphertext_size, value->ciphertext);
    mac_key_data = secure_buffer_data(&mac_key_size, (secure_buffer*)mac_key);
    mac_data = secure_buffer_data(&mac_size, value->mac);

    /* create MAC key. */
    mkey =
        EVP_PKEY_new_mac_key(
            EVP_PKEY_HMAC, NULL, mac_key_data, (int)mac_key_size);
    if (NULL == mkey)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto done;
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
    retval = EVP_DigestSignFinal(mctx, mac_data, &mac_size_out);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_VALUE_MAC;
        goto cleanup_mctx;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto cleanup_mctx;

cleanup_mctx:
    EVP_MD_CTX_free(mctx);

cleanup_mkey:
    EVP_PKEY_free(mkey);

done:
    return retval;
}
