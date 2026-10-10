/**
 * \file database/database_internal.h
 *
 * \brief Internal data structures and functions for interfacing with the
 * database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <lmdb.h>
#include <nepe2/database.h>
#include <rcpr/resource/protected.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief The initial default map size is 10 MB.
 */
#define DATABASE_DEFAULT_MAP_SIZE       (10UL * 1024UL * 1024UL)

/**
 * \brief The increment amount to use when the database runs out of space.
 */
#define DATABASE_MAP_SIZE_INCREMENT     ( 1UL * 1024UL * 1024UL)

/**
 * \brief The first schema version.
 */
#define DATABASE_SCHEMA_VERSION_1       0x00010000

/**
 * \brief The current schema version supported by this library.
 */
#define DATABASE_CURRENT_SCHEMA_VERSION DATABASE_SCHEMA_VERSION_1

struct database
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    MDB_env* env;
    MDB_dbi global_db;
    MDB_dbi metadata_db;
};

struct database_value
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    const secure_buffer* encryption_key;
    secure_buffer* database_key;
    secure_buffer* IV;
    secure_buffer* ciphertext;
    secure_buffer* mac;
};

enum database_global_settings
{
    DATABASE_GLOBAL_SETTING_DATABASE_SIZE                       = 0x00000000,
    DATABASE_GLOBAL_SETTING_SCHEMA                              = 0x00000001,
    DATABASE_GLOBAL_SETTING_SALT                                = 0x00000002,
};

/**
 * \brief Create a \ref database_value instance using the provided hash_id and
 * blob from the database.
 *
 * \note This does not verify that this value is valid. That is up to
 * \ref metadata_from_database_value.
 *
 * \param value             Pointer to the \ref database_value pointer to be set
 *                          with this value on success.
 * \param alloc             The allocator to use for this operation.
 * \param hash_id           The hash_id to use as a key for this record.
 * \param blob              The blob from the database.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_value_create_from_blob(
    database_value** value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* hash_id, const secure_buffer* blob);

/**
 * \brief Release a \ref database instance.
 *
 * \param r             The \ref database \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_resource_release(RCPR_SYM(resource)* r);

/**
 * \brief Release a \ref database_value instance.
 *
 * \param r             The \ref database_value \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_value_resource_release(RCPR_SYM(resource)* r);

/**
 * \brief Using the given read-only transaction, query the size from the global
 * settings table.
 *
 * \param size          Pointer to the size variable set to size on success.
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_get_size(size_t* size, database* db, MDB_txn* txn);

/**
 * \brief Using the given transaction, update the size in the global settings
 * table.
 *
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 * \param size          The new size.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_set_size(database* db, MDB_txn* txn, size_t size);

/**
 * \brief Using the given read-only transaction, query the schema version from
 * the global settings table.
 *
 * \param schema        Pointer to the schema version variable set to the schema
 *                      version of this database on success.
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_get_schema_version(
    uint32_t* schema, database* db, MDB_txn* txn);

/**
 * \brief Using the given transaction, update the schema version in the global
 * settings table.
 *
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 * \param schema        The new schema version.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_set_schema_version(database* db, MDB_txn* txn, uint32_t schema);

/**
 * \brief Generate a legacy database key using the given buffers.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated legacy key on success.
 * \param alloc         The allocator to use for this operation.
 * \param verify        The verification passphrase for this operation.
 * \param master        The master passphrase for this operation.
 * \param session       The session passphrase for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_legacy_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session);

/**
 * \brief Generate a database key using the given buffers.
 *
 * \param key           Pointer to a \ref secure_buffer pointer to receive the
 *                      generated key on success.
 * \param alloc         The allocator to use for this operation.
 * \param verify        The verification passphrase for this operation.
 * \param master        The master passphrase for this operation.
 * \param session       The session passphrase for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_create_key(
    secure_buffer** key, RCPR_SYM(allocator)* alloc,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session);

/**
 * \brief Using the given read-only transaction, query the the database
 * encryption key salt.
 *
 * \param salt          Pointer to the salt buffer pointer to set to the salt
 *                      for this database on success.
 * \param alloc         The \ref allocator to use for this operation.
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_get_encryption_salt(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc, database* db,
    MDB_txn* txn);

/**
 * \brief Using the given transaction, insert a cryptographically random salt
 * into the global settings table.
 *
 * \param salt          Pointer to the salt buffer pointer to set to the salt
 *                      for this database on success.
 * \param alloc         The \ref allocator to use for this operation.
 * \param db            The \ref database for this operation.
 * \param txn           The transaction to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_insert_encryption_salt(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc, database* db,
    MDB_txn* txn);

/**
 * \brief Create a derived key for encryption or message authentication using an
 * encryption key, an IV, and an offset.
 *
 * \param key           Pointer to the \ref secure_buffer pointer to store the
 *                      derived key.
 * \param alloc         The allocator to use for this operation.
 * \param enc           The encryption key to use for this operation.
 * \param IV            The initialization vector to use for this operation.
 * \param offset        The offset to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_derived_key_create(
    secure_buffer** key, RCPR_SYM(allocator)* alloc, const secure_buffer* enc,
    const secure_buffer* IV, uint32_t offset);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
