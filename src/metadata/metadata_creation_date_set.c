/**
 * \file metadata/metadata_creation_date_set.c
 *
 * \brief Set the creation date for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Set the creation date for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param creation_date The creation date for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the creation date to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p creation date field for this \ref metadata instance
 *        is set to \p creation_date.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_creation_date_set(
    metadata* meta, uint64_t creation_date)
{
    meta->creation_date = creation_date;
    meta->creation_date_populated = true;

    return STATUS_SUCCESS;
}
