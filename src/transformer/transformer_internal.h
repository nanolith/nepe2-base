/**
 * \file transformer/transformer_internal.h
 *
 * \brief Internal data types and methods for the transformer interface.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/transformer.h>
#include <rcpr/compare.h>
#include <rcpr/rbtree.h>
#include <rcpr/resource/protected.h>
#include <rcpr/thread.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct transformer
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    char* name;
    transformer_fn xform_fn;
};

struct transformer_registry
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    RCPR_SYM(rbtree)* dict;
    RCPR_SYM(thread_mutex)* mutex;
};

/**
 * \brief Release a \ref transformer instance.
 *
 * \param r             The \ref transformer \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status transformer_resource_release(RCPR_SYM(resource)* r);

/**
 * \brief Get the singleton transformer registry implementation.
 *
 * \param reg           Pointer to the \ref transformer_registry pointer to be
 *                      set with the singleton instance on success.
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
transformer_registry_singleton_get(
    transformer_registry** reg);

/**
 * \brief Release a \ref transformer_registry instance.
 *
 * \param r             The \ref transformer_registry \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status transformer_registry_resource_release(RCPR_SYM(resource)* r);

/**
 * \brief Compare two transformer names.
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
RCPR_SYM(rcpr_comparison_result) transformer_dict_compare(
    void* context, const void* lhs, const void* rhs);

/**
 * \brief Given a \ref transformer \ref resource, return the name as a key.
 *
 * \param context       Unused.
 * \param r             The \ref transformer \ref resource.
 *
 * \returns the name key for this resource.
 */
const void* transformer_dict_key(
    void* context, const RCPR_SYM(resource)* r);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
