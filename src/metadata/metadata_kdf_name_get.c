/**
 * \file metadata/metadata_kdf_name_get.c
 *
 * \brief Get the kdf name for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the kdf name for a given \ref metadata instance.
 *
 * \param kdf_name          Pointer to hold the kdf name pointer on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p kdf_name must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p kdf_name is set to the kdf_name of this instance.
 *      - On failure, \p kdf_name is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_kdf_name_get(
    const char** kdf_name, const metadata* meta)
{
    const char* data;
    size_t size;

    /* verify that the kdf_name field is set. */
    if (NULL == meta->kdf_name)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* get the data for this buffer. */
    data =
        (const char*)secure_buffer_data(&size, (secure_buffer*)&meta->kdf_name);

    /* return the name to the caller. */
    *kdf_name = data;
    return STATUS_SUCCESS;
}
