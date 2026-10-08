/**
 * \file database/database_value_resource_release.c
 *
 * \brief Release a \ref database_value instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

/**
 * \brief Release a \ref database_value instance.
 *
 * \param r             The \ref database_value \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_value_resource_release(RCPR_SYM(resource)* r)
{
    status retval = STATUS_SUCCESS, release_retval;
    database_value* val = (database_value*)r;

    /* cache allocator. */
    rcpr_allocator* alloc = val->alloc;

    /* release database key if set. */
    if (NULL != val->database_key)
    {
        release_retval =
            resource_release(secure_buffer_resource_handle(val->database_key));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* release IV if set. */
    if (NULL != val->IV)
    {
        release_retval =
            resource_release(secure_buffer_resource_handle(val->IV));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* release ciphertext if set. */
    if (NULL != val->ciphertext)
    {
        release_retval =
            resource_release(secure_buffer_resource_handle(val->ciphertext));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* release mac if set. */
    if (NULL != val->mac)
    {
        release_retval =
            resource_release(secure_buffer_resource_handle(val->mac));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* reclaim val memory. */
    explicit_bzero(val, sizeof(*val));
    release_retval = rcpr_allocator_reclaim(alloc, val);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* decode result. */
    return retval;
}
