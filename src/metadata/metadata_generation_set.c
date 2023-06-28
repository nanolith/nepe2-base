/**
 * \file metadata/metadata_generation_set.c
 *
 * \brief Set the generation for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Set the generation for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 * \param generation        The generation for this operation.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the generation to the instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, the \p generation field for this \ref metadata instance is
 *        set to \p generation.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_generation_set(
    metadata* meta, uint32_t generation)
{
    meta->generation = generation;
    meta->generation_populated = true;

    return STATUS_SUCCESS;
}
