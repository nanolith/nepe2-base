/**
 * \file transformer/transformer_registry_resource_release.c
 *
 * \brief A transformer registry can't be released, because it's a singleton.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "transformer_internal.h"

/**
 * \brief Release a \ref transformer_registry instance.
 *
 * \param r             The \ref transformer_registry \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status transformer_registry_resource_release(RCPR_SYM(resource)* r)
{
    (void)r;

    return ERROR_TRANSFORMER_REGISTRY_CANNOT_BE_RELEASED;
}
