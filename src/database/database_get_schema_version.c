/**
 * \file database/database_get_schema_version.c
 *
 * \brief Get the schema version from the global settings table.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/socket_utilities.h>

#include "database_internal.h"

RCPR_IMPORT_socket_utilities;

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
    uint32_t* schema, database* db, MDB_txn* txn)
{
    MDB_val key, val;
    status retval;
    const uint32_t global_setting_key = DATABASE_GLOBAL_SETTING_SCHEMA;

    /* set the key and value for the query. */
    key.mv_size = sizeof(global_setting_key);
    key.mv_data = (void*)&global_setting_key;
    explicit_bzero(&val, sizeof(val));

    /* query the database. */
    retval = mdb_get(txn, db->global_db, &key, &val);
    if (MDB_NOTFOUND == retval)
    {
        retval = ERROR_DATABASE_SCHEMA_NOT_FOUND;
        goto done;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_SCHEMA_QUERY_FAILED;
        goto done;
    }

    /* verify the data size. */
    if (val.mv_size != sizeof(uint32_t))
    {
        retval = ERROR_DATABASE_SCHEMA_BAD;
        goto done;
    }

    /* read the schema version. */
    uint32_t net_schema;
    memcpy(&net_schema, val.mv_data, val.mv_size);
    *schema = socket_utility_ntoh32(net_schema);
    retval = STATUS_SUCCESS;
    goto done;

done:
    explicit_bzero(&key, sizeof(key));
    explicit_bzero(&val, sizeof(val));

    return retval;
}
