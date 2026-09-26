/**
 * \file nepe2/macro_tricks.h
 *
 * \brief Some tricks for tightening up macros.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#ifdef __cplusplus
# define REQUIRE_SEMICOLON_HERE     static_assert(1, "dummy assertion")
#else
# define REQUIRE_SEMICOLON_HERE     _Static_assert(1, "dummy assertion")
#endif
