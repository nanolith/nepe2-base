/**
 * \file database/database_set_size.c
 *
 * \brief Set the size in the global settings table.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/socket_utilities.h>

#include "database_internal.h"

RCPR_IMPORT_socket_utilities;

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
status database_set_size(database* db, MDB_txn* txn, size_t size)
{
    MDB_val key, val;
    status retval;
    const uint32_t global_setting_key = DATABASE_GLOBAL_SETTING_DATABASE_SIZE;

    /* set the key and the value for the query. */
    uint64_t net_size = socket_utility_hton64(size);
    key.mv_size = sizeof(global_setting_key);
    key.mv_data = (void*)&global_setting_key;
    val.mv_size = sizeof(net_size);
    val.mv_data = &net_size;

    /* update the database. */
    retval = mdb_put(txn, db->global_db, &key, &val, 0);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_SIZE_UPDATE;
        goto done;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto done;

done:
    explicit_bzero(&key, sizeof(key));
    explicit_bzero(&val, sizeof(val));

    return retval;
}
