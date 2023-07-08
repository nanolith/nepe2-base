/**
 * \file metadata/metadata_encoding_get.c
 *
 * \brief Get the encoding for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the encoding for a given \ref metadata instance.
 *
 * \param encoding          Pointer to hold the encoding pointer on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p encoding must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p encoding is set to the encoding of this instance.
 *      - On failure, \p encoding is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_encoding_get(
    const char** encoding, const metadata* meta)
{
    const char* data;
    size_t size;

    /* verify that the encoding field is set. */
    if (NULL == meta->encoding)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* get the data for this buffer. */
    data =
        (const char*)secure_buffer_data(&size, (secure_buffer*)&meta->encoding);

    /* return the encoding to the caller. */
    *encoding = data;
    return STATUS_SUCCESS;
}
