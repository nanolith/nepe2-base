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

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
