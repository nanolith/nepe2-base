/**
 * \file database/database_insert_encryption_salt.c
 *
 * \brief Insert the encryption key salt value into the database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include "database_internal.h"

RCPR_IMPORT_resource;

/**
 * \brief Using the given transaction, insert a cryptographically random salt
 * into the global settings table.
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
status database_insert_encryption_salt(
    secure_buffer** salt, RCPR_SYM(allocator)* alloc, database* db,
    MDB_txn* txn)
{
    MDB_val key, val;
    status retval, release_retval;
    secure_buffer* tmp;
    void* tmp_data;
    size_t tmp_size;
    const uint32_t global_setting_key = DATABASE_GLOBAL_SETTING_SALT;

    /* create a buffer for holding the salt. */
    retval = secure_buffer_create(&tmp, alloc, SHA384_DIGEST_LENGTH);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize this buffer with random data. */
    tmp_data = secure_buffer_data(&tmp_size, tmp);
    retval = RAND_bytes(tmp_data, tmp_size);
    if (1 != retval)
    {
        retval = ERROR_DATABASE_SALT_GENERATE_FAILED;
        goto cleanup_tmp;
    }

    /* set the key and value for the upsert. */
    key.mv_size = sizeof(global_setting_key);
    key.mv_data = (void*)&global_setting_key;
    val.mv_size = tmp_size;
    val.mv_data = tmp_data;

    /* update the database. */
    retval = mdb_put(txn, db->global_db, &key, &val, 0);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_DATABASE_SALT_UPDATE;
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

done:
    return retval;
}
