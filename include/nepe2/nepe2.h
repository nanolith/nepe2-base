/**
 * \file nepe2/nepe2.h
 *
 * \brief Base nepe2 library.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/mapper.h>
#include <nepe2/transformer.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief Register all implementations of mappers, transformers, and other
 * extensible mechanisms.
 */
status FN_DECL_MUST_CHECK nepe2_register();

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
