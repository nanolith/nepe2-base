/**
 * \file metadata/metadata_legacy_flag_get.c
 *
 * \brief Get the legacy flag for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the legacy flag for a given \ref metadata instance.
 *
 * \param legacy_flag       Pointer to hold the legacy flag on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p legacy_flag must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p legacy_flag is set to the legacy_flag of this instance.
 *      - On failure, \p legacy_flag is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_legacy_flag_get(
    bool* legacy_flag, const metadata* meta)
{
    /* verify that the legacy flag has been set. */
    if (!meta->legacy_flag_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the legacy flag. */
    *legacy_flag = meta->legacy_flag;
    return STATUS_SUCCESS;
}
