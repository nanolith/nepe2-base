/**
 * \file metadata/metadata_to_buffer.c
 *
 * \brief Serialize a metadata record into a buffer.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <rcpr/socket_utilities.h>
#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_socket_utilities;

/**
 * \brief Serialize a metadata record into a buffer.
 *
 * \param buffer        The pointer to the buffer pointer to hold the serialized
 *                      data on success.
 * \param alloc         The allocator to use for this operation.
 * \param meta          The metadata instance to serialize.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p buffer must be a valid pointer whose pointer value does not
 *        reference a valid buffer.
 *      - \p alloc must reference a valid \ref allocator instance.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p buffer pointer is updated to a pointer to a valid
 *        \ref secure_buffer instance holding the serialized data for the \ref
 *        metadata instance.
 *      - On failure, \p buffer is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_to_buffer(
    secure_buffer** buffer, RCPR_SYM(allocator)* alloc, const metadata* meta)
{
    status retval;
    const uint32_t SERIAL_VERSION = 0x00000001;
    void* hash_id_data = NULL;
    size_t hash_id_size = 0U;
    void* kdf_name_data = NULL;
    size_t kdf_name_size = 0U;
    void* encoding_data = NULL;
    size_t encoding_size = 0U;
    secure_buffer* tmp = NULL;

    /* verify that this record is valid (all fields set). */
    if (metadata_empty_flag_get(meta))
    {
        retval = ERROR_METADATA_FIELD_NOT_SET;
        goto done;
    }

    /* get the data fields for each buffer. */
    hash_id_data = secure_buffer_data(&hash_id_size, meta->hash_id);
    kdf_name_data = secure_buffer_data(&kdf_name_size, meta->kdf_name);
    encoding_data = secure_buffer_data(&encoding_size, meta->encoding);

    /* calculate the size of the serialization record. */
    const size_t serialized_size =
        sizeof(SERIAL_VERSION)
      + sizeof(uint8_t) /* symbolic_encoding. */
      + sizeof(meta->version)
      + sizeof(meta->creation_date)
      + sizeof(meta->revocation_date)
      + sizeof(meta->expiration_date)
      + sizeof(meta->password_length)
      + sizeof(meta->generation)
      + sizeof(uint8_t) /* legacy_flag. */
      + sizeof(uint32_t) /* hash_id_size */
      + sizeof(uint32_t) /* kdf_name_size. */
      + sizeof(uint32_t) /* encoding_size. */
      + hash_id_size
      + kdf_name_size
      + encoding_size;

    /* create a secure buffer instance of this size. */
    retval = secure_buffer_create(&tmp, alloc, serialized_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* make working with this buffer more convenient. */
    size_t dummy_size;
    uint8_t* bptr = secure_buffer_data(&dummy_size, tmp);

    /* write the serial version. */
    uint32_t NET_SERIAL_VERSION = socket_utility_hton32(SERIAL_VERSION);
    memcpy(bptr, &NET_SERIAL_VERSION, sizeof(NET_SERIAL_VERSION));
    bptr += sizeof(NET_SERIAL_VERSION);

    /* write the symbolic encoding. */
    uint8_t net_symbolic_encoding = meta->symbolic_encoding ? 1 : 0;
    *bptr = net_symbolic_encoding;
    bptr += sizeof(net_symbolic_encoding);

    /* write the version. */
    uint32_t net_version = socket_utility_hton32(meta->version);
    memcpy(bptr, &net_version, sizeof(net_version));
    bptr += sizeof(net_version);

    /* write the creation date. */
    uint64_t net_creation_date = socket_utility_hton64(meta->creation_date);
    memcpy(bptr, &net_creation_date, sizeof(net_creation_date));
    bptr += sizeof(net_creation_date);

    /* write the revocation date. */
    uint64_t net_revocation_date = socket_utility_hton64(meta->revocation_date);
    memcpy(bptr, &net_revocation_date, sizeof(net_revocation_date));
    bptr += sizeof(net_revocation_date);

    /* write the expiration date. */
    uint64_t net_expiration_date = socket_utility_hton64(meta->expiration_date);
    memcpy(bptr, &net_expiration_date, sizeof(net_expiration_date));
    bptr += sizeof(net_expiration_date);

    /* write the password length. */
    uint32_t net_password_length = socket_utility_hton32(meta->password_length);
    memcpy(bptr, &net_password_length, sizeof(net_password_length));
    bptr += sizeof(net_password_length);

    /* write the generation. */
    uint32_t net_generation = socket_utility_hton32(meta->generation);
    memcpy(bptr, &net_generation, sizeof(net_generation));
    bptr += sizeof(net_generation);

    /* write the legacy flag. */
    uint8_t net_legacy_flag = meta->legacy_flag ? 1 : 0;
    *bptr = net_legacy_flag;
    bptr += sizeof(net_legacy_flag);

    /* write the hash_id size. */
    uint32_t u32_hash_id_size = hash_id_size; /* shortening cast. */
    RCPR_MODEL_ASSERT(u32_hash_id_size == hash_id_size);
    uint32_t net_hash_id_size = socket_utility_hton32(u32_hash_id_size);
    memcpy(bptr, &net_hash_id_size, sizeof(net_hash_id_size));
    bptr += sizeof(net_hash_id_size);

    /* write the kdf_name size. */
    uint32_t u32_kdf_name_size = kdf_name_size; /* shortening cast. */
    RCPR_MODEL_ASSERT(u32_kdf_name_size == kdf_name_size);
    uint32_t net_kdf_name_size = socket_utility_hton32(u32_kdf_name_size);
    memcpy(bptr, &net_kdf_name_size, sizeof(net_kdf_name_size));
    bptr += sizeof(net_kdf_name_size);

    /* write the encoding_size. */
    uint32_t u32_encoding_size = encoding_size; /* shortening cast. */
    RCPR_MODEL_ASSERT(u32_encoding_size == encoding_size);
    uint32_t net_encoding_size = socket_utility_hton32(u32_encoding_size);
    memcpy(bptr, &net_encoding_size, sizeof(net_encoding_size));
    bptr += sizeof(net_encoding_size);

    /* write the hash_id. */
    memcpy(bptr, hash_id_data, hash_id_size);
    bptr += hash_id_size;

    /* write the kdf name. */
    memcpy(bptr, kdf_name_data, kdf_name_size);
    bptr += kdf_name_size;

    /* write the encoding. */
    memcpy(bptr, encoding_data, encoding_size);
    bptr += encoding_size;

    /* success. */
    retval = STATUS_SUCCESS;
    *buffer = tmp;
    tmp = NULL;
    goto done;
    
done:
    return retval;
}
