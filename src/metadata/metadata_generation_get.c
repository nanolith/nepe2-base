/**
 * \file metadata/metadata_generation_get.c
 *
 * \brief Get the generation for the given \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "metadata_internal.h"

/**
 * \brief Get the generation for a given \ref metadata instance.
 *
 * \param generation        Pointer to hold the generation on success.
 * \param meta              The metadata instance for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_METADATA_FIELD_NOT_SET if the method fails because this field
 *        has not been set.
 *
 * \pre
 *      - \p generation must be a valid pointer.
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - On success, \p generation is set to the generation of this instance.
 *      - On failure, \p generation is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_generation_get(
    uint32_t* generation, const metadata* meta)
{
    /* verify that the generation has been set. */
    if (!meta->generation_populated)
    {
        return ERROR_METADATA_FIELD_NOT_SET;
    }

    /* return the generation. */
    *generation = meta->generation;
    return STATUS_SUCCESS;
}
