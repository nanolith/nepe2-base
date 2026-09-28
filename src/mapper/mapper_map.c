/**
 * \file mapper/mapper_map.c
 *
 * \brief Perform the map operation for the given mapper.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "mapper_internal.h"

/**
 * \brief Perform the map operation using the given mapper, a raw password,
 * and an allocator to produce a secure buffer containing a mapped password.
 *
 * \param mapped        Pointer to the secure buffer pointer to be set to the
 *                      mapped password on success.
 * \param m             The mapper to use for this operation.
 * \param alloc         The allocator to use for this operation.
 * \param raw           The raw password to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
mapper_map(
    secure_buffer** mapped, mapper* m, RCPR_SYM(allocator)* alloc,
    const secure_buffer* raw)
{
    return m->map_fn(mapped, alloc, raw);
}
