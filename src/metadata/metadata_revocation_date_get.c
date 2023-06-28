/**
 * \file metadata/metadata_revocation_date_get.c
 *
 * \brief Get the revocation date for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the revocation date for a given \ref metadata instance.
 *
 * \param revocation_date   Pointer to hold the revocation date on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p revocation_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p revocation_date is set to the revocation date of this
 *        instance.
 *      - On failure, \p revocation_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_revocation_date_get(
    uint64_t* revocation_date, const metadata* meta)
{
    /* verify that the revocation date has been set. */
    if (!meta->revocation_date_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the revocation date. */
    *revocation_date = meta->revocation_date;
    return STATUS_SUCCESS;
}
