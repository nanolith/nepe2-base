/**
 * \file transformer/legacy_transformer_impl.c
 *
 * \brief Implementation of the legacy transformer.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <openssl/sha.h>

#include "transformer_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

/* forward decls. */
static status generate_hash_from_input_buffer(
    secure_buffer** output, rcpr_allocator* alloc, const secure_buffer* input);
static status generate_generation_hash(
    secure_buffer** output, rcpr_allocator* alloc, const metadata* meta);
static status generate_legacy_hash(
    secure_buffer** output, rcpr_allocator* alloc,
    const secure_buffer* master_hash, const secure_buffer* session_hash,
    const secure_buffer* generation_hash);

TRANSFORMER_REGISTER("legacy", &legacy_transformer_function);

/**
 * \brief A transformer function that performs the "legacy" (pre-2.0) password
 * derivation using SHA-256 and simple concatenation.
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
status legacy_transformer_function(
    secure_buffer** raw, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* session,
    const metadata* meta)
{
    status retval, release_retval;
    secure_buffer* master_hash;
    secure_buffer* session_hash;
    secure_buffer* generation_hash = NULL;
    secure_buffer* tmp;

    /* generate the master passphrase hash. */
    retval = generate_hash_from_input_buffer(&master_hash, alloc, master);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* generate the session passphrase hash. */
    retval = generate_hash_from_input_buffer(&session_hash, alloc, session);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_master_hash;
    }

    if (NULL != meta)
    {
        /* generate the generation hash. */
        retval = generate_generation_hash(&generation_hash, alloc, meta);
        if (STATUS_SUCCESS != retval)
        {
            goto cleanup_session_hash;
        }
    }

    /* generate the legacy hash. */
    retval =
        generate_legacy_hash(
            &tmp, alloc, master_hash, session_hash, generation_hash);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_generation_hash;
    }

    /* success. */
    *raw = tmp;
    retval = STATUS_SUCCESS;
    goto cleanup_generation_hash;

cleanup_generation_hash:
    if (NULL != generation_hash)
    {
        release_retval =
            resource_release(secure_buffer_resource_handle(generation_hash));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

cleanup_session_hash:
    release_retval =
        resource_release(secure_buffer_resource_handle(session_hash));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_master_hash:
    release_retval =
        resource_release(secure_buffer_resource_handle(master_hash));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Generate a hash from an input buffer, creating a new output buffer to
 * store it.
 *
 * \param output            Pointer to the secure buffer pointer to store the
 *                          created output buffer on success.
 * \param alloc             The allocator to use for this operation.
 * \param input             The input buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status generate_hash_from_input_buffer(
    secure_buffer** output, rcpr_allocator* alloc, const secure_buffer* input)
{
    status retval;
    secure_buffer* tmp;
    const void* input_data;
    void* output_data;
    size_t input_size, output_size;

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, SHA256_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    input_data = secure_buffer_data(&input_size, (secure_buffer*)input);
    output_data = secure_buffer_data(&output_size, tmp);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input_data, input_size);
    SHA256_Final(output_data, &sha256);
    explicit_bzero(&sha256, sizeof(sha256));

    /* success. */
    retval = STATUS_SUCCESS;
    *output = tmp;
    goto done;

done:
    return retval;
}

/**
 * \brief Generate a hash representing the generation.
 *
 * \param output            Pointer to the secure buffer pointer to store the
 *                          created output buffer on success.
 * \param alloc             The allocator to use for this operation.
 * \param meta              The metadata from which the generation is queried.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status generate_generation_hash(
    secure_buffer** output, rcpr_allocator* alloc, const metadata* meta)
{
    status retval;
    secure_buffer* tmp;
    void* output_data;
    size_t output_size;
    uint32_t generation;

    /* get the generation. */
    retval = metadata_generation_get(&generation, meta);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, SHA256_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    output_data = secure_buffer_data(&output_size, tmp);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &generation, sizeof(generation));
    SHA256_Final(output_data, &sha256);
    explicit_bzero(&sha256, sizeof(sha256));

    /* success. */
    retval = STATUS_SUCCESS;
    *output = tmp;
    goto done;

done:
    generation = 0;

    return retval;
}

/**
 * \brief Generate a legacy password hash.
 *
 * \param output            Pointer to the secure buffer pointer to store the
 *                          created output buffer on success.
 * \param alloc             The allocator to use for this operation.
 * \param master_hash       The master passphrase hash.
 * \param session_hash      The session passphrase hash.
 * \param generation_hash   The password generation hash.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status generate_legacy_hash(
    secure_buffer** output, rcpr_allocator* alloc,
    const secure_buffer* master_hash, const secure_buffer* session_hash,
    const secure_buffer* generation_hash)
{
    status retval;
    secure_buffer* tmp;
    void *output_data, *master_data, *session_data, *generation_data = NULL;
    size_t output_size, master_size, session_size, generation_size;

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, SHA256_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    master_data = secure_buffer_data(&master_size, (secure_buffer*)master_hash);
    session_data =
        secure_buffer_data(&session_size, (secure_buffer*)session_hash);
    output_data = secure_buffer_data(&output_size, tmp);

    /* optionally get generation data. */
    if (NULL != generation_hash)
    {
        generation_data =
            secure_buffer_data(
                &generation_size, (secure_buffer*)generation_hash);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, master_data, master_size);
    SHA256_Update(&sha256, session_data, session_size);

    /* optionally update with generation data. */
    if (NULL != generation_data)
    {
        SHA256_Update(&sha256, generation_data, generation_size);
    }

    /* finalize hash. */
    SHA256_Final(output_data, &sha256);
    explicit_bzero(&sha256, sizeof(sha256));

    /* success. */
    retval = STATUS_SUCCESS;
    *output = tmp;
    goto done;

done:
    return retval;
}
