/**
 * \file nepe2/secure_buffer.h
 *
 * \brief A secure buffer is a fixed size buffer that is erased when its
 * resource handle is released.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
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
 * \brief A metadata structure contains all of the metadata for an entry.
 */
typedef struct metadata metadata;

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Create an empty metadata instance using the given allocator.
 *
 * \param meta          Pointer to the metadata instance.
 * \param alloc         The allocator instance to use for this operation.
 *
 * \note This metadata instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * metadata_resource_handle on this metadata instance.
 *
 * This metadata instance is empty. Empty instances cannot be serialized or
 * accessed. They are empty vessels for serialization.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must not reference a valid \ref metadata instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 * \post
 *      - On success, \p meta is set to a pointer to a valid
 *        \ref metadata instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p meta is not changed and an error status is returned.
 */
status FN_DECL_MUST_CHECK
metadata_create(
    metadata** meta, RCPR_SYM(allocator)* alloc);

/******************************************************************************/
/* Start of accessors.                                                        */
/******************************************************************************/

/**
 * \brief Set the hash id for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param hash_id       The binary hash_id to set for this operation.
 * \param hash_id_size  The size of this hash id.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the hash id to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p hash_id must be a valid pointer pointing to a valid memory region
 *        that is at least \p hash_id_size in length.
 * \post
 *      - On success, the \p hash_id field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_set(
    metadata* meta, const void* hash_id, size_t hash_id_size);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
