/**
 * \file nepe2/terminal.h
 *
 * \brief The terminal interface provides \ref terminal_readpassphrase which
 * reads a passphrase of the given max size, saving it into a \ref
 * secure_buffer instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/secure_buffer.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief Set up the terminal to not echo, then read a passphrase of the given
 * maximum size to a \ref secure_buffer, returning the created \ref
 * secure_buffer on success.
 *
 * \param passphrase        Pointer to a \ref secure_buffer pointer to receive
 *                          the created \ref secure_buffer holding this
 *                          passphrase on success.
 * \param alloc             The allocator to use for this operation.
 * \param max_length        The maximum supported length of the passphrase to be
 *                          read.
 * \param truncate          Set to true if a passphrase exceeding the maximum
 *                          length should be truncated, and false otherwise. If
 *                          false, an error will be returned if a passphrase
 *                          larger than \p max_length is entered.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
terminal_readpassphrase(
    secure_buffer** passphrase, RCPR_SYM(allocator)* alloc, size_t max_length,
    bool truncate);

/**
 * \brief Present a prompt with a sequence of one letter choices, setting the
 * output selection variable based on this choice.
 *
 * \param selection         Pointer to the output variable to be set with this
 *                          choice.
 * \param prompt            The prompt to be presented, without the choices.
 * \param first             The first choice; this should be a character.
 * \param ...               The remaining choices, terminated by -1.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_TERMINAL_BAD_CHOICE if the selection made did not match any of
 *        the choices.
 */
status FN_DECL_MUST_CHECK
terminal_readchoice(
    int* selection, const char* prompt, int first, ...);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
