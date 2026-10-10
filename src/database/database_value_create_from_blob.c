/**
 * \file database/database_value_create_from_blob.c
 *
 * \brief Create a \ref database_value instance from a database blob.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/sha.h>
#include <rcpr/vtable.h>

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;

RCPR_VTABLE resource_vtable database_value_vtable = {
    .release = &database_value_resource_release
};

/**
 * \brief Create a \ref database_value instance using the provided hash_id and
 * blob from the database.
 *
 * \note This does not verify that this value is valid. That is up to
 * \ref metadata_from_database_value.
 *
 * \param value             Pointer to the \ref database_value pointer to be set
 *                          with this value on success.
 * \param alloc             The allocator to use for this operation.
 * \param hash_id           The hash_id to use as a key for this record.
 * \param blob              The blob from the database.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
database_value_create_from_blob(
    database_value** value, RCPR_SYM(allocator)* alloc,
    const secure_buffer* hash_id, const secure_buffer* blob)
{
    status retval, release_retval;
    database_value* tmp;
    const uint8_t *blob_data, *hash_id_data;
    uint8_t *database_key_data, *IV_data,
            *ciphertext_data, *mac_data;
    size_t blob_size, hash_id_size, database_key_size, IV_size, ciphertext_size,
           mac_size;

    /* compute the expected size. */
    const size_t expected_mac_size = SHA384_DIGEST_LENGTH;
    const size_t expected_IV_size = 48;

    /* get parameter buffers. */
    blob_data = secure_buffer_data(&blob_size, (secure_buffer*)blob);
    hash_id_data = secure_buffer_data(&hash_id_size, (secure_buffer*)hash_id);

    /* ensure that this blob is large enough... */
    if (blob_size <= expected_mac_size + expected_IV_size)
    {
        retval = ERROR_DATABASE_VALUE_BAD_BLOB;
        goto done;
    }

    /* compute the ciphertext size. */
    const size_t computed_ciphertext_size =
        blob_size - (expected_mac_size - expected_IV_size);

    /* allocate memory for the value instance. */
    retval = rcpr_allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize resource. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &database_value_vtable);
    tmp->alloc = alloc;

    /* create database key buffer. */
    retval = secure_buffer_create(&tmp->database_key, alloc, hash_id_size);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* create IV buffer. */
    retval = secure_buffer_create(&tmp->IV, alloc, expected_IV_size);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* create ciphertext buffer. */
    retval =
        secure_buffer_create(&tmp->ciphertext, alloc, computed_ciphertext_size);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* create mac buffer. */
    retval = secure_buffer_create(&tmp->mac, alloc, expected_mac_size);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* get buffers. */
    database_key_data =
        secure_buffer_data(&database_key_size, tmp->database_key);
    IV_data = secure_buffer_data(&IV_size, tmp->IV);
    ciphertext_data = secure_buffer_data(&ciphertext_size, tmp->ciphertext);
    mac_data = secure_buffer_data(&mac_size, tmp->mac);

    /* copy data from blob into buffers. */
    memcpy(database_key_data, hash_id_data, database_key_size);
    memcpy(IV_data, blob_data, IV_size);
    blob_data += IV_size;
    memcpy(ciphertext_data, blob_data, ciphertext_size);
    blob_data += ciphertext_size;
    memcpy(mac_data, blob_data, mac_size);
    blob_data += mac_size;

    /* success. */
    *value = tmp;
    retval = STATUS_SUCCESS;
    goto done;

cleanup_tmp:
    release_retval = resource_release(&tmp->hdr);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
