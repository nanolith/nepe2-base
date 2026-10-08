/**
 * \file database/database_upsert_value.c
 *
 * \brief Insert or optionally update a value in the database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/**
 * \brief Insert a value into the database.
 *
 * \param db            The database into which this value is inserted.
 * \param alloc         The allocator to use for this operation.
 * \param value         The value to insert.
 * \param overwrite     The overwrite flag should be set to true to overwrite an
 *                      existing value in the database.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_upsert_value(
    database* db, RCPR_SYM(allocator)* alloc, const database_value* value,
    bool overwrite)
{
    status retval, release_retval;
    unsigned int flags = 0;
    MDB_val key, val;
    MDB_txn* txn;
    secure_buffer* tmp;
    const void *database_key_data, *IV_data, *ciphertext_data, *mac_data;
    uint8_t *tmp_data, *bptr;
    size_t database_key_size, IV_size, ciphertext_size, mac_size, tmp_size;

    /* set the no-overwrite flag if overwrite is false. */
    if (!overwrite)
    {
        flags |= MDB_NOOVERWRITE;
    }

    /* get buffers from the value. */
    database_key_data =
        secure_buffer_data(
            &database_key_size, (secure_buffer*)value->database_key);
    IV_data = secure_buffer_data(&IV_size, (secure_buffer*)value->IV);
    ciphertext_data =
        secure_buffer_data(&ciphertext_size, (secure_buffer*)value->ciphertext);
    mac_data = secure_buffer_data(&mac_size, (secure_buffer*)value->mac);

    /* create the buffer for the database value. */
    tmp_size = IV_size + ciphertext_size + mac_size;
    retval = secure_buffer_create(&tmp, alloc, tmp_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the tmp buffer data. */
    bptr = tmp_data = secure_buffer_data(&tmp_size, tmp);

    /* copy data to tmp buffer. */
    memcpy(bptr, IV_data, IV_size);                     bptr += IV_size;
    memcpy(bptr, ciphertext_data, ciphertext_size);     bptr += ciphertext_size;
    memcpy(bptr, mac_data, mac_size);                   bptr += mac_size;

    /* create a transaction to write this data. */
    retval = mdb_txn_begin(db->env, NULL, 0, &txn);
    if (MDB_MAP_FULL == retval)
    {
        retval = ERROR_DATABASE_MAP_FULL;
        goto cleanup_tmp;
    }
    else if (MDB_MAP_RESIZED == retval)
    {
        retval = ERROR_DATABASE_MAP_RESIZE;
        goto cleanup_tmp;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_METADATA_UPSERT_FAILURE;
        goto cleanup_tmp;
    }

    /* set the key and value for the upsert. */
    key.mv_size = database_key_size;
    key.mv_data = (void*)database_key_data;
    val.mv_size = tmp_size;
    val.mv_data = tmp_data;

    /* put the value into the database. */
    retval = mdb_put(txn, db->metadata_db, &key, &val, flags);
    if (MDB_KEYEXIST == retval)
    {
        retval = ERROR_DATABASE_WOULD_OVERWRITE;
        goto abort_txn;
    }
    else if (MDB_MAP_FULL == retval)
    {
        retval = ERROR_DATABASE_MAP_FULL;
        goto abort_txn;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_METADATA_UPSERT_FAILURE;
        goto abort_txn;
    }

    /* commit transaction. */
    retval = mdb_txn_commit(txn);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_METADATA_UPSERT_FAILURE;
        goto abort_txn;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto cleanup_tmp;

abort_txn:
    mdb_txn_abort(txn);

cleanup_tmp:
    release_retval = resource_release(secure_buffer_resource_handle(tmp));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
