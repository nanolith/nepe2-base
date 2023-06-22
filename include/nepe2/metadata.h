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
#include <stdbool.h>
#include <stdint.h>

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
    metadata* meta);

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
 * instance whole. This setter copies the version to the instance.
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
 * \param version       Pointer to hold the version on success.
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

/**
 * \brief Set the creation date for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param creation_date The creation date for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the creation date to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p creation date field for this \ref metadata instance
 *        is set to \p creation_date.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_creation_date_set(
    metadata* meta, uint64_t creation_date);

/**
 * \brief Get the creation date for a given \ref metadata instance.
 *
 * \param creation_date Pointer to hold the creation date on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p creation_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p creation_date is set to the creation date of this
 *        instance.
 *      - On failure, \p creation_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_creation_date_get(
    uint64_t* creation_date, const metadata* meta);

/**
 * \brief Set the revocation date for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param revocation_date   The revocation date for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the revocation date to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p revocation date field for this \ref metadata
 *        instance is set to \p revocation_date.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_revocation_date_set(
    metadata* meta, uint64_t revocation_date);

/**
 * \brief Get the revocation date for a given \ref metadata instance.
 *
 * \param revocation_date   Pointer to hold the revocation date on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p revocation_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p revocation_date is set to the revocation date of this
 *        instance.
 *      - On failure, \p revocation_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_revocation_date_get(
    uint64_t* revocation_date, const metadata* meta);

/**
 * \brief Set the expiration date for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param expiration_date   The expiration date for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the expiration date to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p expiration date field for this \ref metadata
 *        instance is set to \p expiration_date.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_expiration_date_set(
    metadata* meta, uint64_t expiration_date);

/**
 * \brief Get the expiration date for a given \ref metadata instance.
 *
 * \param expiration_date   Pointer to hold the expiration date on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p expiration_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p expiration_date is set to the expiration date of this
 *        instance.
 *      - On failure, \p expiration_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_expiration_date_get(
    uint64_t* expiration_date, const metadata* meta);

/**
 * \brief Set the password length for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param password_length   The password length for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the password length to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p password_length field for this \ref metadata
 *        instance is set to \p password_length.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_password_length_set(
    metadata* meta, uint32_t password_length);

/**
 * \brief Get the password length for a given \ref metadata instance.
 *
 * \param password_length   Pointer to hold the password length on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p password_length must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p password_length is set to the password length of this
 *        instance.
 *      - On failure, \p password_length is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_password_length_get(
    uint32_t* password_length, const metadata* meta);

/**
 * \brief Set the generation for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param generation        The generation for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the generation to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p generation field for this \ref metadata instance is
 *        set to \p generation.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_generation_set(
    metadata* meta, uint32_t password);

/**
 * \brief Get the generation for a given \ref metadata instance.
 *
 * \param generation        Pointer to hold the generation on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p generation must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p generation is set to the generation of this instance.
 *      - On failure, \p generation is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_generation_get(
    uint32_t* generation, const metadata* meta);

/**
 * \brief Set the legacy flag for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param legacy_flag       The legacy flag for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the legacy flag to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p legacy_flag field for this \ref metadata instance
 *        is set to \p legacy_flag.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_legacy_flag_set(
    metadata* meta, bool legacy_flag);

/**
 * \brief Get the legacy flag for a given \ref metadata instance.
 *
 * \param legacy_flag       Pointer to hold the legacy flag on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p legacy_flag must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p legacy_flag is set to the legacy_flag of this instance.
 *      - On failure, \p legacy_flag is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_legacy_flag_get(
    bool* legacy_flag, const metadata* meta);

/**
 * \brief Get the empty flag for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 *
 * \returns the empty flag.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - returns the empty flag for this \ref metadata instance.
 */
bool
metadata_empty_flag_get(
    const metadata* meta);

/**
 * \brief Set the KDF algorithm name for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param kdf_name      The name of the KDF algorithm for this instance.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the kdf name to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p kdf_name must be a valid pointer pointing to a valid character
 *        string.
 * \post
 *      - On success, the \p kdf_name field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_kdf_name_set(
    metadata* meta, const char* kdf_name);

/**
 * \brief Get the kdf name for a given \ref metadata instance.
 *
 * \param kdf_name          Pointer to hold the kdf name pointer on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p kdf_name must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p kdf_name is set to the kdf_name of this instance.
 *      - On failure, \p kdf_name is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_kdf_name_get(
    const char** kdf_name, const metadata* meta);

/**
 * \brief Set the encoding for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param encoding      The name of the encoding for this instance.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the encoding to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p encoding must be a valid pointer pointing to a valid character
 *        string.
 * \post
 *      - On success, the \p encoding field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_encoding_set(
    metadata* meta, const char* encoding);

/**
 * \brief Get the encoding for a given \ref metadata instance.
 *
 * \param encoding          Pointer to hold the encoding pointer on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p encoding must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p encoding is set to the encoding of this instance.
 *      - On failure, \p encoding is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_encoding_get(
    const char** encoding, const metadata* meta);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
