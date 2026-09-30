/**
 * \file metadata/metadata_iterations_get.c
 *
 * \brief Get the iterations for the given \ref metadata instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the iterations for a given \ref metadata instance.
 *
 * \param iterations        Pointer to be set to the iterations on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p iterations must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p iterations is set to the iterations of this instance.
 *      - On failure, \p iterations is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_iterations_get(
    uint64_t* iterations, const metadata* meta)
{
    /* verify that iterations has been set. */
    if (!meta->iterations_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the creation date. */
    *iterations = meta->iterations;
    return STATUS_SUCCESS;
}
