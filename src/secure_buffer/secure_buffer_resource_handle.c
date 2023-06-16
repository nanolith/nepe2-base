/**
 * \file secure_buffer/secure_buffer_resource_handle.c
 *
 * \brief Get the resource handle for the secure buffer.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "secure_buffer_internal.h"

/**
 * \brief Given a \ref secure_buffer instance, return the resource handle for
 * this \ref secure_buffer instance.
 *
 * \param buffer        The \ref secure_buffer instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref secure_buffer instance.
 */
RCPR_SYM(resource)*
secure_buffer_resource_handle(
    secure_buffer* buffer)
{
    return &buffer->hdr;
}
