/**
 * \file database/database_create_legacy_key.c
 *
 * \brief Create a legacy metadata key for a database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/mapper.h>
#include <nepe2/transformer.h>
#include <openssl/sha.h>

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

/* forward decls. */
static status generate_hash_from_input_buffers(
    secure_buffer** output, rcpr_allocator* alloc,
    const secure_buffer* input1, const secure_buffer* input2);

/**
 * \brief Generate a legacy database key using the given buffers.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated legacy key on success.
 * \param alloc         The allocator to use for this operation.
 * \param verify        The verification passphrase for this operation.
 * \param master        The master passphrase for this operation.
 * \param session       The session passphrase for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_legacy_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session)
{
    status retval, release_retval;
    transformer* xform;
    secure_buffer *vhash, *hash1, *tmp;

    /* Look up the legacy transformer. */
    retval = transformer_registry_lookup(&xform, "legacy");
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the vhash. */
    retval = transformer_transform(&vhash, xform, alloc, master, verify, NULL);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the combined master and session hash. */
    retval = transformer_transform(&hash1, xform, alloc, master, session, NULL);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_vhash;
    }

    /* create the metadata hash. */
    retval = generate_hash_from_input_buffers(&tmp, alloc, hash1, vhash);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_hash1;
    }

    /* success. */
    *key = tmp;
    retval = STATUS_SUCCESS;
    goto cleanup_hash1;

cleanup_hash1:
    release_retval = resource_release(secure_buffer_resource_handle(hash1));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_vhash:
    release_retval = resource_release(secure_buffer_resource_handle(vhash));
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
 * \param input1            Input buffer 1.
 * \param input2            Input buffer 2.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status generate_hash_from_input_buffers(
    secure_buffer** output, rcpr_allocator* alloc,
    const secure_buffer* input1, const secure_buffer* input2)
{
    status retval;
    secure_buffer* tmp;
    const void *input1_data, *input2_data;
    void* output_data;
    size_t input1_size, input2_size, output_size;

    /* create the output buffer. */
    retval = secure_buffer_create(&tmp, alloc, SHA256_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    input1_data = secure_buffer_data(&input1_size, (secure_buffer*)input1);
    input2_data = secure_buffer_data(&input2_size, (secure_buffer*)input2);
    output_data = secure_buffer_data(&output_size, tmp);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input1_data, input1_size);
    SHA256_Update(&sha256, input2_data, input2_size);
    SHA256_Final(output_data, &sha256);
    explicit_bzero(&sha256, sizeof(sha256));

    /* success. */
    retval = STATUS_SUCCESS;
    *output = tmp;
    goto done;

done:
    return retval;
}
