/**
 * \file metadata/metadata_creation_date_get.c
 *
 * \brief Get the creation date for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the creation date for a given \ref metadata instance.
 *
 * \param creation_date Pointer to hold the creation date on success.
 * \param meta          The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p creation_date must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p creation_date is set to the creation date of this
 *        instance.
 *      - On failure, \p creation_date is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_creation_date_get(
    uint64_t* creation_date, const metadata* meta)
{
    /* verify that the creation date has been set. */
    if (!meta->creation_date_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the creation date. */
    *creation_date = meta->creation_date;
    return STATUS_SUCCESS;
}
