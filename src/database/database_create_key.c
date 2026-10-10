/**
 * \file database/database_create_key.c
 *
 * \brief Create a metadata key for a database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/mapper.h>
#include <nepe2/transformer.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/**
 * \brief Generate a database key using the given buffers.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated key on success.
 * \param alloc         The allocator to use for this operation.
 * \param verify        The verification passphrase for this operation.
 * \param master        The master passphrase for this operation.
 * \param session       The session passphrase for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session)
{
    status retval, release_retval;
    transformer* xform;
    secure_buffer *hash1, *tmp;

    /* Look up the transformer. */
    retval = transformer_registry_lookup(&xform, "PBKDF2");
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the combined master and session hash. */
    retval = transformer_transform(&hash1, xform, alloc, master, session, NULL);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create the metadata hash. */
    retval = transformer_transform(&tmp, xform, alloc, hash1, verify, NULL);
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

done:
    return retval;
}
