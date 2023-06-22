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

#include <nepe2/secure_buffer.h>
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

/**
 * \brief Set the hash id from a secure buffer for a given \ref metadata
 * instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param buffer        The \ref secure_buffer instance to use for this setter.
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
 *      - \p buffer must reference a valid \ref secure_buffer instance.
 * \post
 *      - On success, the \p hash_id field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_set_from_secure_buffer(
    metadata* meta, const secure_buffer* buffer);

/**
 * \brief Get the hash id for a given \ref metadata instance.
 *
 * \param hash_id       Pointer to hold the hash_id pointer to hold the hash_id
 *                      on success.
 * \param hash_id_size  Pointer to hold the size of this hash_id on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p hash_id must be a valid pointer.
 *      - \p hash_id_size must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p hash_id field is set to the hash_id of this
 *        instance. This value is owned by this instance and must not be
 *        modified.
 *      - On success, the \p hash_id_size field is set to the size of the
 *        hash_id of this instance.
 *      - On failure, \p hash_id and \p hash_id_size are unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_get(
    const void** hash_id, size_t* hash_id_size, const metadata* meta);

/**
 * \brief Set the version for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param version       The version for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the hash id to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p version field for this \ref metadata instance is
 *        set to \p version.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_version_set(
    metadata* meta, uint32_t version);

/**
 * \brief Get the version for a given \ref metadata instance.
 *
 * \param version       Pointer to hold the version pointer to hold the version
 *                      on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p version must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p version is set to the version of this instance.
 *      - On failure, \p version is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_version_get(
    uint32_t* version, const metadata* meta);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
