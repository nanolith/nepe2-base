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

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
