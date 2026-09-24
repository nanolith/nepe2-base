/**
 * \file transformer/transformer_dict_key.c
 *
 * \brief Get a transformer key for dictionary operations.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "transformer_internal.h"

/**
 * \brief Given a \ref transformer \ref resource, return the name as a key.
 *
 * \param context       Unused.
 * \param r             The \ref transformer \ref resource.
 *
 * \returns the name key for this resource.
 */
const void* transformer_dict_key(
    void* context, const RCPR_SYM(resource)* r)
{
    (void)context;
    const transformer* xform = (const transformer*)r;

    return xform->name;
}
