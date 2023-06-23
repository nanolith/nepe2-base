/**
 * \file metadata/metadata_hash_id_set_from_secure_buffer.c
 *
 * \brief Set the hash_id field for a \ref metadata instance, from a secure
 * buffer.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Set the hash id from a secure buffer for a given \ref metadata
 * instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param buffer        The \ref secure_buffer instance to use for this setter.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the hash id to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p buffer must reference a valid \ref secure_buffer instance.
 * \post
 *      - On success, the \p hash_id field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_hash_id_set_from_secure_buffer(
    metadata* meta, const secure_buffer* buffer)
{
    void* data;
    size_t size;

    /* get the data from the buffer. */
    data = secure_buffer_data(&size, (secure_buffer*)buffer);

    /* set the hash_id. */
    return metadata_hash_id_set(meta, data, size);
}
