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
#include <rcpr/rbtree.h>
#include <rcpr/thread.h>

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

struct mapper_registry
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    RCPR_SYM(rbtree)* dict;
    RCPR_SYM(thread_mutex)* mutex;
};

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

/**
 * \brief Get the singleton mapper registry implementation.
 *
 * \param reg           Pointer to the \ref mapper_registry pointer to be set
 *                      with the singleton instance on success.
 *
 * \note This method optionally allocates a registry instance. This instance
 * exists as a global. This allocation occurs only once, on a single thread. All
 * other threads will receive this single copy.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
mapper_registry_singleton_get(
    mapper_registry** reg);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
