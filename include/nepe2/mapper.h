/**
 * \file nepe2/mapper.h
 *
 * \brief The mapper interface maps a raw passphrase buffer into a password by
 * mapping the raw bits of this buffer into glyphs for the password.
 *
 * Example implementations might be a hex mapper, a Base64 encoder, or a raw
 * translation buffer of one to seven bit values (2 to 128 unique glyphs).
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/macro_tricks.h>
#include <nepe2/metadata.h>
#include <nepe2/secure_buffer.h>
#include <rcpr/allocator.h>
#include <rcpr/resource.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief The mapper is an interface for mapping raw passphrase data into
 * password glyphs.
 */
typedef struct mapper mapper;

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
