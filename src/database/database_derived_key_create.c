/**
 * \file database/database_derived_key_create.c
 *
 * \brief Create a derived key from an encryption key, an IV, and an offset.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/evp.h>
#include <rcpr/socket_utilities.h>

#include "database_internal.h"

RCPR_IMPORT_resource;
RCPR_IMPORT_socket_utilities;

/**
 * \brief Create a derived key for encryption or message authentication using an
 * encryption key, an IV, and an offset.
 *
 * \param key           Pointer to the \ref secure_buffer pointer to store the
 *                      derived key.
 * \param alloc         The allocator to use for this operation.
 * \param enc           The encryption key to use for this operation.
 * \param IV            The initialization vector to use for this operation.
 * \param offset        The offset to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_derived_key_create(
    secure_buffer** key, RCPR_SYM(allocator)* alloc, const secure_buffer* enc,
    const secure_buffer* IV, uint32_t offset)
{
    status retval, release_retval;
    secure_buffer* tmp = NULL;
    const void *enc_data, *IV_data;
    void *tmp_data;
    size_t enc_size, IV_size, tmp_size;
    EVP_MD_CTX* mdctx;

    /* convert the offset to network order. */
    uint32_t net_offset = socket_utility_hton32(offset);

    /* create the buffer for this operation. */
    retval = secure_buffer_create(&tmp, alloc, 32);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the buffers. */
    enc_data = secure_buffer_data(&enc_size, (secure_buffer*)enc);
    IV_data = secure_buffer_data(&IV_size, (secure_buffer*)IV);
    tmp_data = secure_buffer_data(&tmp_size, tmp);

    /* create the message digest context. */
    mdctx = EVP_MD_CTX_new();
    if (NULL == mdctx)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_tmp;
    }

    /* bind this context to SHA-2-512/256. */
    retval = EVP_DigestInit_ex(mdctx, EVP_sha512_256(), NULL);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* add the encryption key data. */
    retval = EVP_DigestUpdate(mdctx, enc_data, enc_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* add the first 32 bytes of the IV. */
    retval = EVP_DigestUpdate(mdctx, IV_data, 32);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* add the offset. */
    retval = EVP_DigestUpdate(mdctx, &net_offset, sizeof(net_offset));
    if (1 != retval)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* finalize the digest.*/
    unsigned int ignored_key_len = 0;
    retval = EVP_DigestFinal_ex(mdctx, tmp_data, &ignored_key_len);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_DERIVED_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* success. */
    *key = tmp;
    tmp = NULL;
    retval = STATUS_SUCCESS;
    goto cleanup_mdctx;

cleanup_mdctx:
    EVP_MD_CTX_free(mdctx);

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
