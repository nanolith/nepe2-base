/**
 * \file nepe2/secure_buffer.h
 *
 * \brief A secure buffer is a fixed size buffer that is erased when its
 * resource handle is released.
 *
 * \copyright 2022 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <rcpr/allocator.h>
#include <rcpr/resource.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief A secure buffer is a buffer of fixed size that is erased when its
 * resource handle is released.
 */
typedef struct secure_buffer secure_buffer;

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Create a secure buffer of the given size using the given allocator.
 *
 * \param buffer        Pointer to the pointer to receive the secure buffer on
 *                      success.
 * \param alloc         The allocator instance to use for this operation.
 * \param size          The size of the secure buffer to allocate.
 *
 * \note This secure buffer is a \ref resource that must be released by calling
 * \ref resource_release on its resource handle when it is no longer needed by
 * the caller. The resource handle can be accessed by calling
 * \ref secure_buffer_resource_handle on this secure buffer instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p buffer must not reference a valid \ref secure_buffer instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 * \post
 *      - On success, \p buffer is set to a pointer to a valid
 *        \ref secure_buffer instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p buffer is not changed and an error status is returned.
 */
status FN_DECL_MUST_CHECK
secure_buffer_create(
    secure_buffer** buffer, RCPR_SYM(allocator)* alloc, size_t size);

/******************************************************************************/
/* Start of accessors.                                                        */
/******************************************************************************/

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
    secure_buffer* buffer);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
