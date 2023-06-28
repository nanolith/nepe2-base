/**
 * \file metadata/metadata_expiration_date_get.c
 *
 * \brief Get the expiration date for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the expiration date for a given \ref metadata instance.
 *
 * \param expiration_date   Pointer to hold the expiration date on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p expiration_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p expiration_date is set to the expiration date of this
 *        instance.
 *      - On failure, \p expiration_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_expiration_date_get(
    uint64_t* expiration_date, const metadata* meta)
{
    /* verify that the expiration date has been set. */
    if (!meta->expiration_date_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the expiration date. */
    *expiration_date = meta->expiration_date;
    return STATUS_SUCCESS;
}
