/**
 * \file database/database_check_or_insert_schema.c
 *
 * \brief Check the schema version in the database, inserting a schema version
 * if none is available.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

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
status database_check_or_insert_schema(database* db)
{
    status retval;
    MDB_txn* txn;
    uint32_t schema;
    bool update_schema = false;

    /* begin a read-only transaction to query the database schema. */
    retval = mdb_txn_begin(db->env, NULL, MDB_RDONLY, &txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_BEGIN;
        goto done;
    }

    /* get the current database schema. */
    retval = database_get_schema_version(&schema, db, txn);
    if (ERROR_DATABASE_SCHEMA_NOT_FOUND == retval)
    {
        update_schema = true;
    }
    else if (STATUS_SUCCESS != retval)
    {
        goto txn_abort;
    }

    /* we are done with this transaction. */
    mdb_txn_abort(txn);

    /* should we update the schema version? */
    if (update_schema)
    {
        /* begin a read-write transaction. */
        retval = mdb_txn_begin(db->env, NULL, 0, &txn);
        if (STATUS_SUCCESS != retval)
        {
            retval = ERROR_DATABASE_MDB_TXN_BEGIN;
            goto done;
        }

        /* update the schema version. */
        schema = DATABASE_CURRENT_SCHEMA_VERSION;
        retval = database_set_schema_version(db, txn, schema);
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

    /* check the schema version. */
    if (schema < DATABASE_CURRENT_SCHEMA_VERSION)
    {
        /* we need to upgrade the schema before continuing. */
        retval = ERROR_DATABASE_SCHEMA_NEEDS_UPGRADE;
    }
    else if (schema > DATABASE_CURRENT_SCHEMA_VERSION)
    {
        /* the schema is newer than this library. */
        retval = ERROR_DATABASE_SCHEMA_UNSUPPORTED;
    }
    else
    {
        /* the database schema is compatible with this library. */
        retval = STATUS_SUCCESS;
    }

    /* there is no active transaction, so skip the abort. */
    goto done;

txn_abort:
    mdb_txn_abort(txn);

done:
    return retval;
}
