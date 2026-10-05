/**
 * \file database/database_create_encryption_key.c
 *
 * \brief Create the encryption key for the database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <nepe2/transformer.h>
#include <openssl/evp.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/**
 * \brief Create the encryption key for the database from a master passphrase
 * and a salt.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated encryption key on success.
 * \param alloc         The allocator to use for this operation.
 * \param master        The master passphrase for this operation.
 * \param salt          The salt for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_encryption_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* salt)
{
    status retval, release_retval;
    transformer* xform;
    secure_buffer *kdf, *tmp = NULL;
    void *kdf_data, *tmp_data;
    size_t kdf_size, tmp_size;

    /* Look up the PBKDF2 transformer. */
    retval = transformer_registry_lookup(&xform, "PBKDF2");
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the combined master and salt hash. */
    /* NOTE: the salt and master buffers are _purposefully_ switched. */
    retval = transformer_transform(&kdf, xform, alloc, salt, master, NULL);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, 32);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_kdf;
    }

    /* get the buffers. */
    kdf_data = secure_buffer_data(&kdf_size, kdf);
    tmp_data = secure_buffer_data(&tmp_size, tmp);

    /* Create a message digest context. */
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (NULL == mdctx)
    {
        retval = ERROR_DATABASE_ENCRYPTION_KEY_CREATE;
        goto cleanup_tmp;
    }

    /* Bind this context to SHA-2-512/256. */
    retval = EVP_DigestInit_ex(mdctx, EVP_sha512_256(), NULL);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_ENCRYPTION_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* Reduce this down to a 256-bit key. */
    retval = EVP_DigestUpdate(mdctx, kdf_data, kdf_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_ENCRYPTION_KEY_CREATE;
        goto cleanup_mdctx;
    }

    /* Finalize this digest. */
    unsigned int ignored_key_len = 0;
    retval = EVP_DigestFinal_ex(mdctx, tmp_data, &ignored_key_len);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_ENCRYPTION_KEY_CREATE;
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

cleanup_kdf:
    release_retval = resource_release(secure_buffer_resource_handle(kdf));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
