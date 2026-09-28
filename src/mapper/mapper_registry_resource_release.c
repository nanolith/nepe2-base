/**
 * \file mapper/mapper_registry_resource_release.c
 *
 * \brief A mapper registry can't be released, because it's a singleton.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "mapper_internal.h"

/**
 * \brief Release a \ref mapper_registry instance.
 *
 * \param r             The \ref mapper_registry \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status mapper_registry_resource_release(RCPR_SYM(resource)* r)
{
    (void)r;

    return ERROR_MAPPER_REGISTRY_CANNOT_BE_RELEASED;
}
