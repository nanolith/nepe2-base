/**
 * \file secure_buffer/secure_buffer_internal.h
 *
 * \brief Internal header for \ref secure_buffer.
 *
 * \copyright 2022-2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/secure_buffer.h>
#include <rcpr/resource/protected.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct secure_buffer
{
    RCPR_SYM(resource) hdr;
    RCPR_MODEL_STRUCT_TAG(secure_buffer);
    RCPR_SYM(allocator)* alloc;
    size_t size;
    void* data;
};

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
status secure_buffer_resource_release(RCPR_SYM(resource)* r);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
