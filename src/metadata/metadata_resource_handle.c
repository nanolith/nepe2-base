/**
 * \file metadata/metadata_resource_handle.c
 *
 * \brief Get the resource handle for a \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Given a \ref metadata instance, return the resource handle for this
 * \ref metadata instance.
 *
 * \param meta          The \ref metadata instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref metadata instance.
 */
RCPR_SYM(resource)*
metadata_resource_handle(
    metadata* meta)
{
    return &meta->hdr;
}
