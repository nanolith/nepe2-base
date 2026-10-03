/**
 * \file database/database_get_size.c
 *
 * \brief Get the size from the global settings table.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/socket_utilities.h>

#include "database_internal.h"

RCPR_IMPORT_socket_utilities;

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
status database_get_size(size_t* size, database* db, MDB_txn* txn)
{
    MDB_val key, val;
    status retval;
    const uint32_t global_setting_key = DATABASE_GLOBAL_SETTING_DATABASE_SIZE;

    /* set the key and value for the query. */
    key.mv_size = sizeof(global_setting_key);
    key.mv_data = (void*)&global_setting_key;
    explicit_bzero(&val, sizeof(val));

    /* query the database. */
    retval = mdb_get(txn, db->global_db, &key, &val);
    if (MDB_NOTFOUND == retval)
    {
        retval = ERROR_DATABASE_SIZE_NOT_FOUND;
        goto done;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_SIZE_QUERY_FAILED;
        goto done;
    }

    /* verify the data size. */
    if (val.mv_size != sizeof(uint64_t))
    {
        retval = ERROR_DATABASE_SIZE_BAD;
        goto done;
    }

    /* read the size. */
    uint64_t net_size;
    memcpy(&net_size, val.mv_data, val.mv_size);
    *size = socket_utility_ntoh64(net_size);
    retval = STATUS_SUCCESS;
    goto done;

done:
    explicit_bzero(&key, sizeof(key));
    explicit_bzero(&val, sizeof(val));

    return retval;
}
