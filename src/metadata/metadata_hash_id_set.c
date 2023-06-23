/**
 * \file metadata/metadata_hash_id_set.c
 *
 * \brief Set the hash_id field for a \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Set the hash id for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param hash_id       The binary hash_id to set for this operation.
 * \param hash_id_size  The size of this hash id.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the hash id to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p hash_id must be a valid pointer pointing to a valid memory region
 *        that is at least \p hash_id_size in length.
 * \post
 *      - On success, the \p hash_id field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_set(
    metadata* meta, const void* hash_id, size_t hash_id_size)
{
    status retval;
    void* data;
    size_t size;
    secure_buffer* tmp = NULL;
    secure_buffer* tmp2 = NULL;

    /* create a secure buffer instance. */
    retval = secure_buffer_create(&tmp, meta->alloc, hash_id_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the data pointer. */
    data = secure_buffer_data(&size, tmp);

    /* copy the hash id. */
    memcpy(data, hash_id, size);

    /* cache the old hash_id, if set. */
    tmp2 = meta->hash_id;

    /* set the new hash_id. */
    meta->hash_id = tmp;

    /* release the old hash_id if set. */
    if (NULL != tmp2)
    {
        retval = resource_release(secure_buffer_resource_handle(tmp2));
        if (STATUS_SUCCESS != retval)
        {
            goto done;
        }
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
