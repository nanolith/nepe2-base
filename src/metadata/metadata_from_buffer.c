/**
 * \file metadata/metadata_from_buffer.c
 *
 * \brief Read a metadata record from a buffer.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/socket_utilities.h>
#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_resource;
RCPR_IMPORT_socket_utilities;

/**
 * \brief Serialize a metadata record from a buffer.
 *
 * \param meta          Pointer to hold the \ref metadata instance pointer on
 *                      success.
 * \param alloc         The allocator to use for this operation.
 * \param buffer        The buffer to read this instance from.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *      - ERROR_METADATA_SERIALIZATION_FAILED if the serialization failed.
 *
 * \pre
 *      - \p meta must be a valid pointer whose pointer value does not
 *        reference a valid \ref metadata instance.
 *      - \p alloc must reference a valid \ref allocator instance.
 *      - \p buffer must reference a valid \ref secure_buffer instance.
 * \post
 *      - On success, the \p meta pointer is updated to a pointer to a valid
 *        \ref metadata instance holding the parsed data from the
 *        \ref secure_buffer instance.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_from_buffer(
    metadata** meta, RCPR_SYM(allocator)* alloc, const secure_buffer* buffer)
{
    status retval, release_retval;
    const uint32_t SERIAL_VERSION_1 = 0x00000001;
    const uint8_t* bptr = NULL;
    size_t buffer_size = 0U;
    metadata* tmp = NULL;
    char kdf_name_buffer[1024];
    char encoding_buffer[1024];

    /* get the buffer pointer and size. */
    bptr =
        (const uint8_t*)secure_buffer_data(
            &buffer_size, (secure_buffer*)buffer);

    /* verify that the size is long enough to get the version. */
    if (buffer_size < sizeof(uint32_t))
    {
        retval = ERROR_METADATA_INVALID_BUFFER_SIZE;
        goto done;
    }

    /* read the serial version. */
    uint32_t net_serial_version;
    memcpy(&net_serial_version, bptr, sizeof(net_serial_version));
    uint32_t serial_version = socket_utility_ntoh32(net_serial_version);

    /* we currently only support serial version 1. */
    if (SERIAL_VERSION_1 != serial_version)
    {
        retval = ERROR_METADATA_UNKNOWN_SERIAL_VERSION;
        goto done;
    }

    /* compute the header size for serial version 1. */
    const size_t serialized_header_size =
        sizeof(net_serial_version)
      + sizeof(uint8_t) /* symbolic_encoding. */
      + sizeof(tmp->version)
      + sizeof(tmp->creation_date)
      + sizeof(tmp->revocation_date)
      + sizeof(tmp->expiration_date)
      + sizeof(tmp->password_length)
      + sizeof(tmp->generation)
      + sizeof(uint8_t)   /* legacy_flag. */
      + sizeof(uint32_t)  /* hash_id_size */
      + sizeof(uint32_t)  /* kdf_name_size. */
      + sizeof(uint32_t); /* encoding_size. */

    /* verify that the buffer size is at least the header size. */
    if (buffer_size < serialized_header_size)
    {
        retval = ERROR_METADATA_INVALID_BUFFER_SIZE;
        goto done;
    }

    /* create a metadata instance. */
    retval = metadata_create(&tmp, alloc);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* skip past the serial version. */
    bptr += sizeof(net_serial_version);
    buffer_size -= sizeof(net_serial_version);

    /* read the symbolic encoding. */
    uint8_t symbolic_encoding = *bptr;
    bptr += sizeof(symbolic_encoding);
    buffer_size -= sizeof(symbolic_encoding);
    /* TODO - verify this against the symbolic_encoding flag below. */

    /* read the version. */
    uint32_t net_version;
    memcpy(&net_version, bptr, sizeof(net_version));
    bptr += sizeof(net_version);
    buffer_size -= sizeof(net_version);

    /* set the version. */
    retval = metadata_version_set(tmp, socket_utility_ntoh32(net_version));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the creation date. */
    uint64_t net_creation_date;
    memcpy(&net_creation_date, bptr, sizeof(net_creation_date));
    bptr += sizeof(net_creation_date);
    buffer_size -= sizeof(net_creation_date);

    /* set the creation date. */
    retval =
        metadata_creation_date_set(
            tmp, socket_utility_ntoh64(net_creation_date));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the revocation date. */
    uint64_t net_revocation_date;
    memcpy(&net_revocation_date, bptr, sizeof(net_revocation_date));
    bptr += sizeof(net_revocation_date);
    buffer_size -= sizeof(net_revocation_date);

    /* set the revocation date. */
    retval =
        metadata_revocation_date_set(
            tmp, socket_utility_ntoh64(net_revocation_date));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the expiration date. */
    uint64_t net_expiration_date;
    memcpy(&net_expiration_date, bptr, sizeof(net_expiration_date));
    bptr += sizeof(net_expiration_date);
    buffer_size -= sizeof(net_expiration_date);

    /* set the expiration date. */
    retval =
        metadata_expiration_date_set(
            tmp, socket_utility_ntoh64(net_expiration_date));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the password length. */
    uint32_t net_password_length;
    memcpy(&net_password_length, bptr, sizeof(net_password_length));
    bptr += sizeof(net_password_length);
    buffer_size -= sizeof(net_password_length);

    /* set the password length. */
    retval =
        metadata_password_length_set(
            tmp, socket_utility_ntoh32(net_password_length));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the generation. */
    uint32_t net_generation;
    memcpy(&net_generation, bptr, sizeof(net_generation));
    bptr += sizeof(net_generation);
    buffer_size -= sizeof(net_generation);

    /* set the generation. */
    retval =
        metadata_generation_set(tmp, socket_utility_ntoh32(net_generation));
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the legacy flag. */
    uint32_t legacy_flag = *bptr;
    bptr += sizeof(legacy_flag);
    buffer_size -= sizeof(legacy_flag);

    /* set the legacy flag. */
    retval = metadata_legacy_flag_set(tmp, legacy_flag ? true : false);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* read the hash_id size. */
    uint32_t net_hash_id_size;
    memcpy(&net_hash_id_size, bptr, sizeof(net_hash_id_size));
    bptr += sizeof(net_hash_id_size);
    buffer_size -= sizeof(net_hash_id_size);
    uint32_t hash_id_size = socket_utility_ntoh32(net_hash_id_size);

    /* read the kdf_name_size. */
    uint32_t net_kdf_name_size;
    memcpy(&net_kdf_name_size, bptr, sizeof(net_kdf_name_size));
    bptr += sizeof(net_kdf_name_size);
    buffer_size -= sizeof(net_kdf_name_size);
    uint32_t kdf_name_size = socket_utility_ntoh32(net_kdf_name_size);

    /* read the encoding_size. */
    uint32_t net_encoding_size;
    memcpy(&net_encoding_size, bptr, sizeof(net_encoding_size));
    bptr += sizeof(net_encoding_size);
    buffer_size -= sizeof(net_encoding_size);
    uint32_t encoding_size = socket_utility_ntoh32(net_encoding_size);

    /* verify that the remaining buffer is large enough for our variable length
     * fields. */
    if (buffer_size != hash_id_size + kdf_name_size + encoding_size)
    {
        retval = ERROR_METADATA_INVALID_BUFFER_SIZE;
        goto cleanup_tmp;
    }

    /* set the hash id. */
    retval = metadata_hash_id_set(tmp, bptr, hash_id_size);
    bptr += hash_id_size;
    buffer_size -= hash_id_size;
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* sanity check on kdf name size. */
    if (kdf_name_size >= sizeof(kdf_name_buffer))
    {
        retval = ERROR_METADATA_INVALID_BUFFER_SIZE;
        goto cleanup_tmp;
    }

    /* read the kdf name. */
    memcpy(kdf_name_buffer, bptr, kdf_name_size);
    bptr += kdf_name_size;
    buffer_size -= kdf_name_size;

    /* ASCIIZ the buffer. */
    kdf_name_buffer[kdf_name_size] = 0;

    /* set the kdf name. */
    retval = metadata_kdf_name_set(tmp, kdf_name_buffer);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* sanity check on encoding size. */
    if (encoding_size >= sizeof(encoding_buffer))
    {
        retval = ERROR_METADATA_INVALID_BUFFER_SIZE;
        goto cleanup_tmp;
    }

    /* read the encoding. */
    memcpy(encoding_buffer, bptr, encoding_size);
    bptr += encoding_size;
    buffer_size -= encoding_size;

    /* ASCIIZ the buffer. */
    encoding_buffer[encoding_size] = 0;

    /* set the encoding. */
    retval = metadata_encoding_set(tmp, encoding_buffer);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* verify that this metadata instance is now valid. */
    if (!metadata_empty_flag_get(tmp))
    {
        retval = ERROR_METADATA_FIELD_NOT_SET;
        goto cleanup_tmp;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    *meta = tmp;
    tmp = NULL;
    goto done;

cleanup_tmp:
    if (NULL != tmp)
    {
        release_retval = resource_release(&tmp->hdr);
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

done:
    memset(kdf_name_buffer, 0, sizeof(kdf_name_buffer));
    memset(encoding_buffer, 0, sizeof(encoding_buffer));

    return retval;
}
