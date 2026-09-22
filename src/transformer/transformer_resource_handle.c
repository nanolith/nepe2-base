/**
 * \file transformer/transformer_resource_handle.c
 *
 * \brief Return the resource handle of a transformer.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "transformer_internal.h"

/**
 * \brief Given a \ref transformer instance, return the resource handle for this
 * \ref transformer instance.
 *
 * \param xform         The \ref transformer instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref transformer instance.
 */
RCPR_SYM(resource)*
transformer_resource_handle(
    transformer* xform)
{
    return &xform->hdr;
}
