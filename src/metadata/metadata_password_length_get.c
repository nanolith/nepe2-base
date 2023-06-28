/**
 * \file metadata/metadata_password_length_get.c
 *
 * \brief Get the password length for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the password length for a given \ref metadata instance.
 *
 * \param password_length   Pointer to hold the password length on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p password_length must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p password_length is set to the password length of this
 *        instance.
 *      - On failure, \p password_length is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_password_length_get(
    uint32_t* password_length, const metadata* meta)
{
    /* verify that the password length has been set. */
    if (!meta->password_length_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the password length. */
    *password_length = meta->password_length;
    return STATUS_SUCCESS;
}
