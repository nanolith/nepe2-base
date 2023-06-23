/**
 * \file metadata/metadata_resource_release.c
 *
 * \brief Release a \ref metadata resource.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Release a \ref metadata resource.
 *
 * \param r             Pointer to the \ref metadata resource to be released.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - an error code on failure.
 */
status metadata_resource_release(RCPR_SYM(resource)* r)
{
    status hash_id_release_retval = STATUS_SUCCESS;
    status kdf_name_release_retval = STATUS_SUCCESS;
    status encoding_release_retval = STATUS_SUCCESS;
    status reclaim_retval = STATUS_SUCCESS;

    metadata* meta = (metadata*)r;

    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(prop_metadata_valid(meta));

    /* cache allocator. */
    allocator* alloc = meta->alloc;

    /* release hash_id if set. */
    if (NULL != meta->hash_id)
    {
        hash_id_release_retval =
            resource_release(secure_buffer_resource_handle(meta->hash_id));
    }

    /* release kdf_name if set. */
    if (NULL != meta->kdf_name)
    {
        kdf_name_release_retval =
            resource_release(secure_buffer_resource_handle(meta->kdf_name));
    }

    /* release encoding if set. */
    if (NULL != meta->encoding)
    {
        encoding_release_retval =
            resource_release(secure_buffer_resource_handle(meta->encoding));
    }

    /* clear memory. */
    RCPR_MODEL_EXEMPT(memset(meta, 0, sizeof(*meta)));

    /* reclaim memory. */
    reclaim_retval = allocator_reclaim(alloc, meta);

    /* decode response code. */
    if (STATUS_SUCCESS != hash_id_release_retval)
    {
        return hash_id_release_retval;
    }
    else if (STATUS_SUCCESS != kdf_name_release_retval)
    {
        return kdf_name_release_retval;
    }
    else if (STATUS_SUCCESS != encoding_release_retval)
    {
        return encoding_release_retval;
    }
    else
    {
        return reclaim_retval;
    }
}
