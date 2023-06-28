/**
 * \file metadata/metadata_legacy_flag_get.c
 *
 * \brief Set the legacy flag for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Set the legacy flag for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param legacy_flag       The legacy flag for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the legacy flag to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p legacy_flag field for this \ref metadata instance
 *        is set to \p legacy_flag.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_legacy_flag_set(
    metadata* meta, bool legacy_flag)
{
    meta->legacy_flag = legacy_flag;
    meta->legacy_flag_populated = true;

    return STATUS_SUCCESS;
}
