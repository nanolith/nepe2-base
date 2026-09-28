/**
 * \file transformer/transformer_transform.c
 *
 * \brief Perform the transform operation for the given transformer.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "transformer_internal.h"

/**
 * \brief Perform the transform operation using the given transformer, a master
 * passphrase, a session passphrase, and metadata to produce a secure buffer
 * containing a raw generated password.
 *
 * \param raw           Pointer to the secure buffer pointer to be set to the
 *                      raw generated password on success.
 * \param xform         The transformer to use for this operation.
 * \param alloc         The allocator to use for this operation.
 * \param master        The master passphrase to use for this operation.
 * \param session       The session passphrase to use for this operation.
 * \param meta          The metadata to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_transform(
    secure_buffer** raw, transformer* xform, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* session,
    const metadata* meta)
{
    return xform->xform_fn(raw, alloc, master, session, meta);
}
