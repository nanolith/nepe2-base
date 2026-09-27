/**
 * \file mapper/mapper_resource_release.c
 *
 * \brief Release a mapper resource.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "mapper_internal.h"

RCPR_IMPORT_allocator_as(rcpr);

/**
 * \brief Release a \ref mapper instance.
 *
 * \param r             The \ref mapper \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status mapper_resource_release(RCPR_SYM(resource)* r)
{
    status retval = STATUS_SUCCESS, release_retval;
    mapper* m = (mapper*)r;

    /* cache allocator. */
    rcpr_allocator* alloc = m->alloc;

    /* reclaim name if set. */
    if (NULL != m->name)
    {
        explicit_bzero(m->name, strlen(m->name));
        release_retval = rcpr_allocator_reclaim(alloc, m->name);
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

    /* reclaim m. */
    explicit_bzero(m, sizeof(*m));
    release_retval = rcpr_allocator_reclaim(alloc, m);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* decode result. */
    return retval;
}
