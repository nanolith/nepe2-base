/**
 * \file transformer/transformer_resource_release.c
 *
 * \brief Release a transformer resource.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "transformer_internal.h"

RCPR_IMPORT_allocator_as(rcpr);

/**
 * \brief Release a \ref transformer instance.
 *
 * \param r             The \ref transformer \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status transformer_resource_release(RCPR_SYM(resource)* r)
{
    status retval = STATUS_SUCCESS, release_retval;
    transformer* xform = (transformer*)r;

    /* cache allocator. */
    rcpr_allocator* alloc = xform->alloc;

    /* reclaim name if set. */
    if (NULL != xform->name)
    {
        explicit_bzero(xform->name, strlen(xform->name));
        release_retval = rcpr_allocator_reclaim(alloc, xform->name);
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* reclaim xform. */
    explicit_bzero(xform, sizeof(*xform));
    release_retval = rcpr_allocator_reclaim(alloc, xform);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* decode result. */
    return retval;
}
