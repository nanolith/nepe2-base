/**
 * \file nepe2/database.h
 *
 * \brief Data structures and functions for interfacing with the database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

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

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
