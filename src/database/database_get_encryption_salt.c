/**
 * \file database/database_get_encryption_salt.c
 *
 * \brief Get the encryption key salt for this database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

/**
 * \brief Using the given read-only transaction, query the schema version from
 * the database encryption key salt.
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
    MDB_txn* txn)
{
    MDB_val key, val;
    status retval;
    secure_buffer* tmp;
    void* tmp_data;
    size_t tmp_size;
    const uint32_t global_setting_key = DATABASE_GLOBAL_SETTING_SALT;

    /* set the key and value for the query. */
    key.mv_size = sizeof(global_setting_key);
    key.mv_data = (void*)&global_setting_key;
    explicit_bzero(&val, sizeof(val));

    /* query the database. */
    retval = mdb_get(txn, db->global_db, &key, &val);
    if (MDB_NOTFOUND == retval)
    {
        retval = ERROR_DATABASE_SALT_NOT_FOUND;
        goto done;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_SALT_QUERY_FAILED;
        goto done;
    }

    /* create a buffer for storing this salt. */
    retval = secure_buffer_create(&tmp, alloc, val.mv_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* copy the salt. */
    tmp_data = secure_buffer_data(&tmp_size, tmp);
    memcpy(tmp_data, val.mv_data, tmp_size);

    /* success. */
    *salt = tmp;
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
