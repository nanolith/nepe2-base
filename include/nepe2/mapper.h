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

/**
 * \brief A mapper function takes an allocator and a raw generated password to
 * produce a secure buffer containing a password that has been mapped to a given
 * translation set or encoding.
 *
 * \param mapped        Pointer to the secure buffer pointer to be set to the
 *                      mapped password on success.
 * \param alloc         The allocator to use for this operation.
 * \param raw           The raw password to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
typedef status (*mapper_fn)(
    secure_buffer** mapped, RCPR_SYM(allocator)* alloc,
    const secure_buffer* raw);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
