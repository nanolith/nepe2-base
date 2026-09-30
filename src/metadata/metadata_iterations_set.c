/**
 * \file metadata/metadata_iterations_set.c
 *
 * \brief Set the iterations for the given \ref metadata instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Set the iterations for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param iterations    The number of iterations for the password KDF operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - non-zero on failure.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p iterations field for this \ref metadata instance is
 *        set.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_iterations_set(
    metadata* meta, uint64_t iterations)
{
    meta->iterations = iterations;
    meta->iterations_populated = true;

    return STATUS_SUCCESS;
}
