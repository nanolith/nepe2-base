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

/**
 * \brief Release a \ref mapper_registry instance.
 *
 * \param r             The \ref mapper_registry \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status mapper_registry_resource_release(RCPR_SYM(resource)* r);

/**
 * \brief Compare two mapper names.
 *
 * \param context       Unused.
 * \param lhs           The left-hand side of the comparison.
 * \param rhs           The right-hand side of the comparison.
 *
 * \returns an integer value representing the comparison result.
 *      - RCPR_COMPARE_LT if \p lhs &lt; \p rhs.
 *      - RCPR_COMPARE_EQ if \p lhs == \p rhs.
 *      - RCPR_COMPARE_GT if \p lhs &gt; \p rhs.
 */
RCPR_SYM(rcpr_comparison_result) mapper_dict_compare(
    void* context, const void* lhs, const void* rhs);

/**
 * \brief Given a \ref mapper \ref resource, return the name as a key.
 *
 * \param context       Unused.
 * \param r             The \ref mapper \ref resource.
 *
 * \returns the name key for this resource.
 */
const void* mapper_dict_key(
    void* context, const RCPR_SYM(resource)* r);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
