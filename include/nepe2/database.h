/**
 * \file nepe2/database.h
 *
 * \brief Data structures and functions for interfacing with the database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/metadata.h>
#include <nepe2/secure_buffer.h>
#include <rcpr/allocator.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief The database provides routines for storing and retrieving metadata and
 * defaults.
 */
typedef struct database database;

/**
 * \brief A database value opaque instance.
 */
typedef struct database_value database_value;

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Open a database instance backed by the given directory name.
 *
 * \param db            Pointer to the database pointer to be set to the
 *                      created database instance on success.
 * \param alloc         The allocator to use for this operation.
 * \param dir_name      The directory name for this instance.
 *
 * \note This database instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * database_resource_handle on this database instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 *
 * \pre
 *      - \p db must not reference a valid \ref database instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 *      - \p dir_name must be a valid string and must not be NULL.
 * \post
 *      - On success, \p db is set to a pointer to a valid \ref database
 *      instance, which is a \ref resource owned by the caller that must be
 *      released when no longer needed.
 *      - On failure, \p db is set to NULL and an error status is returned.
 */
status FN_DECL_MUST_CHECK
database_open(
    database** db, RCPR_SYM(allocator)* alloc, const char* dir_name);

/**
 * \brief The extended form of the database open constructor allows for the
 * initial map size to be explicitly set. This is used largely for testing.
 *
 * \param db            Pointer to the database pointer to be set to the
 *                      created database instance on success.
 * \param alloc         The allocator to use for this operation.
 * \param dir_name      The directory name for this instance.
 * \param map_size      The initial map size to use.
 *
 * \note This database instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * database_resource_handle on this database instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 *
 * \pre
 *      - \p db must not reference a valid \ref database instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 *      - \p dir_name must be a valid string and must not be NULL.
 * \post
 *      - On success, \p db is set to a pointer to a valid \ref database
 *      instance, which is a \ref resource owned by the caller that must be
 *      released when no longer needed.
 *      - On failure, \p db is set to NULL and an error status is returned.
 */
status FN_DECL_MUST_CHECK
database_open_ex(
    database** db, RCPR_SYM(allocator)* alloc, const char* dir_name,
    size_t map_size);

/**
 * \brief Create a \ref database_value instance from the given verification
 * passphrase, master passphrase, session passphrase, encryption key, and
 * metadata.
 *
 * \note The legacy flag in the metadata controls whether this is a legacy
 * Nepephemeral 1 entry or a modern metadata entry. This, in turn, controls the
 * database key that is generated.
 *
 * \param value             Pointer to the \ref database_value pointer to be set
 *                          with this value on success.
 * \param alloc             The allocator to use for this operation.
 * \param meta              The metadata to use to create this value.
 * \param verify            The verification passphrase for this operation.
 * \param master            The master passphrase for this operation.
 * \param session           The session passphrase for this operation.
 * \param encryption_key    The encryption key for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_value_create(
    database_value** value, RCPR_SYM(allocator)* alloc, const metadata* meta,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session, const secure_buffer* encryption_key);

/**
 * \brief Create a \ref database_value instance from the given verification
 * passphrase, master passphrase, hash id, encryption key, and
 * metadata.
 *
 * \param value             Pointer to the \ref database_value pointer to be set
 *                          with this value on success.
 * \param alloc             The allocator to use for this operation.
 * \param meta              The metadata to use to create this value.
 * \param verify            The verification passphrase for this operation.
 * \param master            The master passphrase for this operation.
 * \param hash_id           The hash_id to use as a key for this record.
 * \param encryption_key    The encryption key for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_value_create_with_hash_id(
    database_value** value, RCPR_SYM(allocator)* alloc, const metadata* meta,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* hash_id, const secure_buffer* encryption_key);

/******************************************************************************/
/* Start of accessors.                                                        */
/******************************************************************************/

/**
 * \brief Given a \ref database instance, return the resource handle for this
 * \ref database instance.
 *
 * \param db            The \ref database instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref database instance.
 */
RCPR_SYM(resource)*
database_resource_handle(database* db);

/**
 * \brief Given a \ref database_value instance, return the resource handle for
 * this \ref database_value instance.
 *
 * \param db            The \ref database_value instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref database instance.
 */
RCPR_SYM(resource)*
database_value_resource_handle(database_value* val);

/**
 * \brief Update the database size to the currently stored size in the global
 * settings table, or to the next increment of size.
 *
 * \param db            The \ref database for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_update_size(database* db);

/**
 * \brief Check or insert the schema version into the database.
 *
 * \param db            The \ref database for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_DATABASE_SCHEMA_NEEDS_UPGRADE if the schema version is older
 *        than what this library supports and thus needs a call to \ref
 *        database_upgrade to continue.
 *      - ERROR_DATABASE_SCHEMA_UNSUPPORTED if the schema version is newer than
 *        what this library supports and thus it is the library that needs
 *        upgrading as not to inadvertently damage the integrity of this
 *        database.
 *      - a non-zero error code on failure.
 */
status database_check_or_insert_schema(database* db);

/**
 * \brief Get or insert the encryption key salt value into the global settings
 * table.
 *
 * \param salt          Pointer to the salt buffer pointer to set to the salt
 *                      for this database on success.
 * \param alloc         The \ref allocator to use for this operation.
 * \param db            The \ref database for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_get_or_insert_encryption_salt(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc, database* db);

/**
 * \brief Create the encryption key for the database from a master passphrase
 * and a salt.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated encryption key on success.
 * \param alloc         The allocator to use for this operation.
 * \param master        The master passphrase for this operation.
 * \param salt          The salt for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_encryption_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* salt);

/**
 * \brief Insert a value into the database.
 *
 * \param db            The database into which this value is inserted.
 * \param alloc         The allocator to use for this operation.
 * \param value         The value to insert.
 * \param overwrite     The overwrite flag should be set to true to overwrite an
 *                      existing value in the database.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_upsert_value(
    database* db, RCPR_SYM(allocator)* alloc, const database_value* value,
    bool overwrite);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
