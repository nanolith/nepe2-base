/**
 * \file metadata/metadata_kdf_name_set.c
 *
 * \brief Set the kdf_name field for a \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Set the KDF algorithm name for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param kdf_name      The name of the KDF algorithm for this instance.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the kdf name to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p kdf_name must be a valid pointer pointing to a valid character
 *        string.
 * \post
 *      - On success, the \p kdf_name field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_kdf_name_set(
    metadata* meta, const char* kdf_name)
{
    status retval;
    void* data;
    size_t size;
    secure_buffer* tmp = NULL;
    secure_buffer* tmp2 = NULL;
    size_t kdf_name_length = strlen(kdf_name);

    /* create a secure buffer instance. */
    retval = secure_buffer_create(&tmp, meta->alloc, kdf_name_length + 1);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the data pointer. */
    data = secure_buffer_data(&size, tmp);

    /* copy the kdf name. */
    memset(data, 0, kdf_name_length + 1);
    memcpy(data, kdf_name, kdf_name_length);

    /* cache the old kdf_name if set. */
    tmp2 = meta->kdf_name;

    /* set the new kdf_name. */
    meta->kdf_name = tmp;

    /* release the old kdf_name if set. */
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
