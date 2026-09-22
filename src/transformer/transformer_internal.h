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
#include <rcpr/resource/protected.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct transformer
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    const char* name;
    transformer_fn xform_fn;
};

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
