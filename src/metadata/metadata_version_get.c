/**
 * \file metadata/metadata_version_get.c
 *
 * \brief Get the version for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the version for a given \ref metadata instance.
 *
 * \param version       Pointer to hold the version on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p version must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p version is set to the version of this instance.
 *      - On failure, \p version is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_version_get(
    uint32_t* version, const metadata* meta)
{
    if (meta->version_populated)
    {
        *version = meta->version;
        return STATUS_SUCCESS;
    }
    else
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }
}
