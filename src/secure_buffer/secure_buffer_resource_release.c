/**
 * \file secure_buffer/secure_resource_release.c
 *
 * \brief Release a secure buffer resource.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <string.h>

#include "secure_buffer_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Release a \ref secure_buffer resource.
 *
 * \param r             Pointer to the \ref secure_buffer resource to be
 *                      released.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - an error code on failure.
 */
status secure_buffer_resource_release(RCPR_SYM(resource)* r)
{
    status data_reclaim_retval = STATUS_SUCCESS;
    status reclaim_retval = STATUS_SUCCESS;

    /* reverse type erasure. */
    secure_buffer* buffer = (secure_buffer*)r;

    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(prop_secure_buffer_valid(buffer));

    /* cache the allocator. */
    allocator* alloc = buffer->alloc;

    /* if the buffer is set, reclaim it. */
    if (NULL != buffer->data)
    {
        memset(buffer->data, 0, buffer->size);
        data_reclaim_retval = allocator_reclaim(alloc, buffer->data);
    }

    /* clear memory. */
    memset(buffer, 0, sizeof(*buffer));

    /* reclaim memory. */
    reclaim_retval = allocator_reclaim(alloc, buffer);

    /* decode return value. */
    if (STATUS_SUCCESS != data_reclaim_retval)
    {
        return data_reclaim_retval;
    }
    else
    {
        return reclaim_retval;
    }
}
