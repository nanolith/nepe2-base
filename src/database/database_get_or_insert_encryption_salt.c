/**
 * \file database/database_get_or_insert_encryption_salt.c
 *
 * \brief Get or insert the encryption salt for this database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

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
    secure_buffer** salt, RCPR_SYM(allocator)* alloc, database* db)
{
    status retval, release_retval;
    MDB_txn* txn;
    secure_buffer* tmp;

    /* begin a read-only transaction to query the database encryption salt. */
    retval = mdb_txn_begin(db->env, NULL, MDB_RDONLY, &txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_BEGIN;
        goto done;
    }

    /* attempt to get the database encryption salt. */
    retval = database_get_encryption_salt(salt, alloc, db, txn);
    if (ERROR_DATABASE_SALT_NOT_FOUND != retval)
    {
        /* this operation either succeeded or failed for a reason other than not
         * found. Return the status code after aborting the read transaction. */
        goto txn_abort;
    }

    /* abort the read-only transaction and start a read-write transaction. */
    mdb_txn_abort(txn);
    retval = mdb_txn_begin(db->env, NULL, 0, &txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_BEGIN;
        goto done;
    }

    /* insert a new salt. */
    retval = database_insert_encryption_salt(&tmp, alloc, db, txn);
    if (STATUS_SUCCESS != retval)
    {
        goto txn_abort;
    }

    /* commit this insertion. */
    retval = mdb_txn_commit(txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_MDB_TXN_COMMIT;
        goto cleanup_tmp;
    }

    /* success. */
    *salt = tmp;
    retval = STATUS_SUCCESS;
    goto done;

cleanup_tmp:
    release_retval = resource_release(secure_buffer_resource_handle(tmp));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

txn_abort:
    mdb_txn_abort(txn);

done:
    return retval;
}
