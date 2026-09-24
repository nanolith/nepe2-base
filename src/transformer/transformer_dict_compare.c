/**
 * \file transformer/transformer_dict_compare.c
 *
 * \brief Compare two transformers by name.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "transformer_internal.h"

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
    void* context, const void* lhs, const void* rhs)
{
    (void)context;
    const char* l = (const char*)lhs;
    const char* r = (const char*)rhs;

    int retval = strcmp(l, r);

    if (retval < 0)
    {
        return RCPR_COMPARE_LT;
    }
    else if (retval > 0)
    {
        return RCPR_COMPARE_GT;
    }
    else
    {
        return RCPR_COMPARE_EQ;
    }
}
