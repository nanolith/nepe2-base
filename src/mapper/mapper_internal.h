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
};

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
