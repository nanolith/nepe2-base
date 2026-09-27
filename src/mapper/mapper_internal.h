/**
 * \file mapper/mapper_internal.h
 *
 * \brief Internal data types and methods for the mapper interface.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/mapper.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct mapper
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    char* name;
    mapper_fn map_fn;
};

/**
 * \brief The mapper registry allows mappers to be registered and
 * queried by name.
 */
typedef struct mapper_registry mapper_registry;

/**
 * \brief Release a \ref mapper instance.
 *
 * \param r             The \ref mapper \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status mapper_resource_release(RCPR_SYM(resource)* r);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
