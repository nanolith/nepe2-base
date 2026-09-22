/**
 * \file nepe2/transformer.h
 *
 * \brief The transformer interface provides a means of turning a master
 * passphrase, a session passphrase, and metadata into a secure buffer that
 * contains raw password information.
 *
 * The transformer corresponds roughly to the KDF algorithm defined by the
 * metadata.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/metadata.h>
#include <nepe2/secure_buffer.h>
#include <rcpr/allocator.h>
#include <rcpr/resource.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief The transformer is an interface for transforming passphrases into
 * passwords.
 */
typedef struct transformer transformer;

/**
 * \brief A transformer function takes an allocator, a master passphrase, a
 * session passphrase, and metadata to produce a secure buffer containing a
 * raw generated password.
 *
 * \param raw           Pointer to the secure buffer pointer to be set to the
 *                      raw generated password on success.
 * \param alloc         The allocator to use for this operation.
 * \param master        The master passphrase to use for this operation.
 * \param session       The session passphrase to use for this operation.
 * \param meta          The metadata to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
typedef status (*transformer_fn)(
    secure_buffer** raw, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* session,
    const metadata* meta);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
