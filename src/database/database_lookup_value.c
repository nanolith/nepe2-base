/**
 * \file database/database_lookup_value.c
 *
 * \brief Look up a value from the metadata table.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/* forward decls. */
static status database_lookup_value_common(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* database_key);
static status database_lookup_current_value(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session);
static status database_lookup_legacy_value(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session);

/**
 * \brief Look up a database value using the given verification passphrase,
 * master passphrase, and session passphrase.
 *
 * \note If \p legacy_fb is true, the legacy metadata will be considered if the
 * initial lookup fails.
 *
 * \param value         Pointer to the \ref database_value pointer to update
 *                      with the created \ref database_value on success.
 * \param alloc         The allocator to use for this operation.
 * \param db            The database from which this value is read.
 * \param verify        The verification passphrase for this entry.
 * \param master        The master passphrase for this entry.
 * \param session       The session passphrase for this entry.
 * \param legacy_fb     If true, fall back to legacy metadata if the query
 *                      fails.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_lookup_value(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session, bool legacy_fb)
{
    status retval;

    /* try looking up the current value. */
    retval =
        database_lookup_current_value(
            value, alloc, db, verify, master, session);
    if (ERROR_DATABASE_NOT_FOUND != retval)
    {
        goto done;
    }

    /* fall back to legacy? */
    if (legacy_fb)
    {
        retval =
            database_lookup_legacy_value(
                value, alloc, db, verify, master, session);
    }

    /* regardless, exit. */
    goto done;

done:
    return retval;
}

/**
 * \brief Look up a database value by deriving a current style key.
 *
 * \param value         Pointer to the \ref database_value pointer to update
 *                      with the created \ref database_value on success.
 * \param alloc         The allocator to use for this operation.
 * \param db            The database from which this value is read.
 * \param verify        The verification passphrase for this entry.
 * \param master        The master passphrase for this entry.
 * \param session       The session passphrase for this entry.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status database_lookup_current_value(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session)
{
    status retval, release_retval;
    secure_buffer* key;

    /* derive the database key. */
    retval = database_create_key(&key, alloc, verify, master, session);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* perform the common database lookup operation. */
    retval = database_lookup_value_common(value, alloc, db, key);

    /* clean up the database key on the way out. */
    goto cleanup_key;

cleanup_key:
    release_retval = resource_release(secure_buffer_resource_handle(key));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Look up a database value by deriving a legacy style key.
 *
 * \param value         Pointer to the \ref database_value pointer to update
 *                      with the created \ref database_value on success.
 * \param alloc         The allocator to use for this operation.
 * \param db            The database from which this value is read.
 * \param verify        The verification passphrase for this entry.
 * \param master        The master passphrase for this entry.
 * \param session       The session passphrase for this entry.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status database_lookup_legacy_value(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* verify, const secure_buffer* master,
    const secure_buffer* session)
{
    status retval, release_retval;
    secure_buffer* key;

    /* derive the database key. */
    retval = database_create_legacy_key(&key, alloc, verify, master, session);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* perform the common database lookup operation. */
    retval = database_lookup_value_common(value, alloc, db, key);

    /* clean up the database key on the way out. */
    goto cleanup_key;

cleanup_key:
    release_retval = resource_release(secure_buffer_resource_handle(key));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief Common database lookup logic.
 *
 * \param value         Pointer to the \ref database_value pointer to update
 *                      with the created \ref database_value on success.
 * \param alloc         The allocator to use for this operation.
 * \param db            The database from which this value is read.
 * \param database_key  The database key to use for this query.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status database_lookup_value_common(
    database_value** value, RCPR_SYM(allocator)* alloc, database* db,
    const secure_buffer* database_key)
{
    status retval, release_retval;
    MDB_val key, val;
    MDB_txn* txn;
    secure_buffer* blob;
    const uint8_t *key_data;
    uint8_t *blob_data;
    size_t key_size, blob_size;;

    /* get key buffer. */
    key_data = secure_buffer_data(&key_size, (secure_buffer*)database_key);

    /* create a read transaction to read this data. */
    retval = mdb_txn_begin(db->env, NULL, MDB_RDONLY, &txn);
    if (MDB_MAP_FULL == retval)
    {
        retval = ERROR_DATABASE_MAP_FULL;
        goto done;
    }
    else if (MDB_MAP_RESIZED == retval)
    {
        retval = ERROR_DATABASE_MAP_RESIZE;
        goto done;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_METADATA_LOOKUP_FAILURE;
        goto done;
    }

    /* set the key and value for this query. */
    key.mv_size = key_size;
    key.mv_data = (void*)key_data;
    explicit_bzero(&val, sizeof(val));

    /* get the value from the database. */
    retval = mdb_get(txn, db->metadata_db, &key, &val);
    if (MDB_NOTFOUND == retval)
    {
        retval = ERROR_DATABASE_NOT_FOUND;
        goto abort_txn;
    }
    else if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_METADATA_LOOKUP_FAILURE;
        goto abort_txn;
    }

    /* create a buffer for holding the blob. */
    retval = secure_buffer_create(&blob, alloc, val.mv_size);
    if (STATUS_SUCCESS != retval)
    {
        goto abort_txn;
    }

    /* get blob buffer data. */
    blob_data = secure_buffer_data(&blob_size, blob);

    /* copy the data. */
    memcpy(blob_data, val.mv_data, blob_size);

    /* create the database value from this blob. */
    retval = database_value_create_from_blob(value, alloc, database_key, blob);

    /* clean up on the way out. */
    goto cleanup_blob;

cleanup_blob:
    release_retval = resource_release(secure_buffer_resource_handle(blob));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

abort_txn:
    mdb_txn_abort(txn);

done:
    return retval;
}
