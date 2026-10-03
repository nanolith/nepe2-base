/**
 * \file database/database_open.c
 *
 * \brief Open a database instance backed by the given directory name.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/vtable.h>

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

RCPR_VTABLE
resource_vtable database_vtable = {
    .release = &database_resource_release
};

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
    database** db, RCPR_SYM(allocator)* alloc, const char* dir_name)
{
    return database_open_ex(db, alloc, dir_name, DATABASE_DEFAULT_MAP_SIZE);
}

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
    size_t map_size)
{
    status retval, release_retval;
    MDB_txn* txn;
    database* tmp = NULL;

    /* allocate memory for the database structure. */
    retval = rcpr_allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize resource and set vtable. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &database_vtable);
    tmp->alloc = alloc;

    /* create the LMDB environment. */
    retval = mdb_env_create(&tmp->env);
    if (STATUS_SUCCESS != retval)
    {
        tmp->env = NULL;
        retval = ERROR_DATABASE_MDB_ENV_CREATE;
        goto cleanup_tmp;
    }

    /* set the database map size. */
    retval = mdb_env_set_mapsize(tmp->env, map_size);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_ENV_SET_MAPSIZE;
        goto cleanup_tmp;
    }

    /* set the maximum number of databases (global and metadata). */
    retval = mdb_env_set_maxdbs(tmp->env, 2);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_ENV_SET_MAXDBS;
        goto cleanup_tmp;
    }

    /* open the environment. */
    retval = mdb_env_open(tmp->env, dir_name, 0, 0600);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_ENV_OPEN;
        goto cleanup_tmp;
    }

    /* begin transaction for opening databases. */
    retval = mdb_txn_begin(tmp->env, NULL, 0, &txn);
    if (MDB_MAP_FULL == retval || MDB_MAP_RESIZED == retval)
    {
        /* attempt to resize the database. */
        retval = mdb_env_set_mapsize(tmp->env, 0);
        if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_MDB_TXN_BEGIN;
            goto cleanup_tmp;
        }

        /* retry the transaction. */
        retval = mdb_txn_begin(tmp->env, NULL, 0, &txn);
        if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_MDB_TXN_BEGIN;
            goto cleanup_tmp;
        }
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_BEGIN;
        goto cleanup_tmp;
    }

    /* open the global database. */
    retval = mdb_dbi_open(txn, "global.db", MDB_CREATE, &tmp->global_db);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_DBI_OPEN;
        goto abort_txn;
    }

    /* open the metadata database. */
    retval = mdb_dbi_open(txn, "metadata.db", MDB_CREATE, &tmp->metadata_db);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_DBI_OPEN;
        goto abort_txn;
    }

    /* commit the transaction. */
    retval = mdb_txn_commit(txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_COMMIT;
        goto cleanup_tmp;
    }

    /* update the mapped database size based on the previously stored value. */
    retval = database_update_size(tmp);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MAP_RESIZE_FAILURE;
        goto cleanup_tmp;
    }

    /* success. */
    *db = tmp;
    retval = STATUS_SUCCESS;
    goto done;

abort_txn:
    mdb_txn_abort(txn);

cleanup_tmp:
    release_retval = resource_release(&tmp->hdr);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
