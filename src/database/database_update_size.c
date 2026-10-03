/**
 * \file database/database_update_size.c
 *
 * \brief Update the size of the database to reflect the size in the global
 * settings, bumping if necessary.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

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
status database_update_size(database* db)
{
    status retval;
    MDB_envinfo stat;
    MDB_txn* txn;
    int retries = 0;

    while (retries < 3)
    {
        bool update_size = false;
        size_t computed_size = DATABASE_DEFAULT_MAP_SIZE;

        /* get the map size from the environment. */
        retval = mdb_env_info(db->env, &stat);
        if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_ENVINFO;
            goto done;
        }

        /* begin a read-only transaction to query the database size. */
        retval = mdb_txn_begin(db->env, NULL, MDB_RDONLY, &txn);
        if (MDB_MAP_RESIZED == retval)
        {
            goto resize;
        }
        else if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_MDB_TXN_BEGIN;
            goto done;
        }

        /* get the current database size. */
        retval = database_get_size(&computed_size, db, txn);
        if (ERROR_DATABASE_SIZE_NOT_FOUND == retval)
        {
            update_size = true;
        }
        else if (STATUS_SUCCESS != retval)
        {
            goto txn_abort;
        }

        /* we are done with this transaction. */
        mdb_txn_abort(txn);

        /* should we bump our map size? */
        if (stat.me_mapsize < computed_size)
        {
            retval = mdb_env_set_mapsize(db->env, computed_size);
            if (STATUS_SUCCESS != retval)
            {
                retval = ERROR_DATABASE_MDB_ENV_SET_MAPSIZE;
                goto done;
            }
        }

        /* should we update the size in global settings? */
        if (update_size)
        {
            /* begin an update transaction. */
            retval = mdb_txn_begin(db->env, NULL, 0, &txn);
            if (MDB_MAP_RESIZED == retval)
            {
                goto resize;
            }
            else if (STATUS_SUCCESS != retval)
            {
                retval = ERROR_DATABASE_MDB_TXN_BEGIN;
                goto done;
            }

            /* update the size. */
            retval = database_set_size(db, txn, computed_size);
            if (STATUS_SUCCESS != retval)
            {
                goto txn_abort;
            }

            /* commit the transaction. */
            retval = mdb_txn_commit(txn);
            if (STATUS_SUCCESS != retval)
            {
                retval = ERROR_DATABASE_MDB_TXN_COMMIT;
                goto txn_abort;
            }
        }

        /* success. */
        retval = STATUS_SUCCESS;
        goto done;

    resize:
        /* attempt to resize the database. */
        retval = mdb_env_set_mapsize(db->env, 0);
        if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_MDB_ENV_SET_MAPSIZE;
            goto done;
        }

        retries += 1;
        continue;
    }

txn_abort:
    mdb_txn_abort(txn);

done:
    return retval;
}
