/**
 * \file transformer/PKDF2_transformer_impl.c
 *
 * \brief Implementation of the PBKDF2 transformer.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <rcpr/socket_utilities.h>

#include "transformer_internal.h"

RCPR_IMPORT_resource;
RCPR_IMPORT_socket_utilities;

TRANSFORMER_REGISTER("PBKDF2", &PBKDF2_transformer_function);

/* forward decls. */
static status salt_secure_buffer_create(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const metadata* meta);

/**
 * \brief A transformer function that performs the PKDF2 password derivation
 * using SHA-384 and simple concatenation.
 *
 * \note This transformer is provided for historical purposes, but it is
 * deprecated.
 *
 * \param raw       Pointer to the secure buffer pointer to be set to the raw
 *                  generated password on success.
 * \param alloc     The allocator to use for this operation.
 * \param master    The master passphrase to use for this operation.
 * \param session   The session passphrase to use for this operation.
 * \param meta      The metadata to use for this operation, ignored if NULL.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status PBKDF2_transformer_function(
    secure_buffer** raw, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* session,
    const metadata* meta)
{
    status retval, release_retval;
    secure_buffer *salt, *tmp;
    void *output_data, *salt_data, *session_data;
    size_t output_size, salt_size, session_size;
    uint64_t iterations = 500000;

    /* create the salt secure buffer. */
    retval = salt_secure_buffer_create(&salt, alloc, master, meta);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the output secure buffer. */
    retval = secure_buffer_create(&tmp, alloc, SHA384_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_salt;
    }

    /* get the buffer data pointers. */
    output_data = secure_buffer_data(&output_size, tmp);
    salt_data = secure_buffer_data(&salt_size, salt);
    session_data = secure_buffer_data(&session_size, (secure_buffer*)session);

    /* if meta is available, update the number of iterations. */
    if (NULL != meta)
    {
        retval = metadata_iterations_get(&iterations, meta);
        if (STATUS_SUCCESS != retval)
        {
            goto cleanup_tmp;
        }
    }

    /* derive the raw password. */
    retval =
        PKCS5_PBKDF2_HMAC(
            (const char*)session_data, session_size,
            (const unsigned char*)salt_data, salt_size, iterations,
            EVP_sha384(), output_size, (unsigned char*)output_data);
    if (1 != retval)
    {
        goto cleanup_tmp;
    }

    /* success. */
    *raw = tmp;
    retval = STATUS_SUCCESS;
    goto cleanup_salt;

cleanup_tmp:
    release_retval = resource_release(secure_buffer_resource_handle(tmp));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_salt:
    release_retval = resource_release(secure_buffer_resource_handle(salt));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Create a secure buffer to hold the salt for this transformation.
 *
 * \param salt              The buffer that will hold the salt for this
 *                          transformation.
 * \param alloc             The allocator to use for this operation.
 * \param master            The master passphrase to use for this operation.
 * \param meta              The metadata to use for this operation, or NULL.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status salt_secure_buffer_create(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const metadata* meta)
{
    status retval;
    void *master_data, *salt_data;
    size_t master_size, salt_size;
    uint32_t net_generation;
    secure_buffer* tmp;

    /* get the master buffer data. */
    master_data = secure_buffer_data(&master_size, (secure_buffer*)master);

    /* compute the salt size. */
    salt_size = master_size;
    if (NULL != meta)
    {
        uint32_t generation;
        retval = metadata_generation_get(&generation, meta);
        if (STATUS_SUCCESS != retval)
        {
            goto done;
        }

        net_generation = socket_utility_hton32(generation);
        salt_size += sizeof(net_generation);
    }

    /* create salt buffer. */
    retval = secure_buffer_create(&tmp, alloc, salt_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get salt buffer data. */
    salt_data = secure_buffer_data(&salt_size, tmp);

    /* make working with this buffer more convenient. */
    uint8_t* buffer = (uint8_t*)salt_data;

    /* copy master passphrase. */
    memcpy(buffer, master_data, master_size);
    buffer += master_size;

    /* copy generation data. */
    if (NULL != meta)
    {
        memcpy(buffer, &net_generation, sizeof(net_generation));
        buffer += sizeof(net_generation);
    }

    /* success. */
    retval = STATUS_SUCCESS;
    *salt = tmp;
    goto done;

done:
    return retval;
}
