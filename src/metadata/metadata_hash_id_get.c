/**
 * \file metadata/metadata_hash_id_get.c
 *
 * \brief Get the hash_id field for a \ref metadata instance, if set.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the hash id for a given \ref metadata instance.
 *
 * \param hash_id       Pointer to hold the hash_id pointer to hold the hash_id
 *                      on success.
 * \param hash_id_size  Pointer to hold the size of this hash_id on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p hash_id must be a valid pointer.
 *      - \p hash_id_size must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p hash_id field is set to the hash_id of this
 *        instance. This value is owned by this instance and must not be
 *        modified.
 *      - On success, the \p hash_id_size field is set to the size of the
 *        hash_id of this instance.
 *      - On failure, \p hash_id and \p hash_id_size are unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_get(
    const void** hash_id, size_t* hash_id_size, const metadata* meta)
{
    void* data;
    size_t size;

    /* check to see if hash_id set. */
    if (NULL == meta->hash_id)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* get the data from the buffer. */
    data = secure_buffer_data(&size, (secure_buffer*)meta->hash_id);

    /* return the hash_id and hash_id_size. */
    *hash_id = data;
    *hash_id_size = size;

    return STATUS_SUCCESS;
}
