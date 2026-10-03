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
 * \brief The increment amount to use when the database runs out of space.
 */
#define DATABASE_MAP_SIZE_INCREMENT     (1UL * 1024UL * 1024UL)

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
    const secure_buffer* database_key;
    secure_buffer* IV;
    secure_buffer* ciphertext;
    secure_buffer* mac;
};

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

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
