/**
 * \file mapper/mapper_resource_handle.c
 *
 * \brief Return the \ref resource handle for a given \ref mapper instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "mapper_internal.h"

/**
 * \brief Given a \ref mapper instance, return the resource handle for this
 * \ref mapper instance.
 *
 * \param m             The \ref mapper instance from which the resource handle
 *                      is returned.
 *
 * \returns the resource handle for this \ref mapper instance.
 */
RCPR_SYM(resource)*
mapper_resource_handle(mapper* m)
{
    return &m->hdr;
}
