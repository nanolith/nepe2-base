/**
 * \file mapper/mapper_dict_key.c
 *
 * \brief Get a mapper key for dictionary operations.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "mapper_internal.h"

/**
 * \brief Given a \ref mapper \ref resource, return the name as a key.
 *
 * \param context       Unused.
 * \param r             The \ref mapper \ref resource.
 *
 * \returns the name key for this resource.
 */
const void* mapper_dict_key(
    void* context, const RCPR_SYM(resource)* r)
{
    (void)context;
    const mapper* xform = (const mapper*)r;

    return xform->name;
}
