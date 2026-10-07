/**
 * \file terminal/terminal_readchoice.h
 *
 * \brief Read a choice from the terminal.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <nepe2/terminal.h>
#include <stdio.h>
#include <stdarg.h>

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
    int* selection, const char* prompt, int first, ...)
{
    int arg = first;
    va_list prompt_list, choice_list;

    va_start(prompt_list, first);
    va_copy(choice_list, prompt_list);

    fpurge(stdin);

    printf("%s [%c", prompt, arg);

    while (-1 != arg)
    {
        arg = va_arg(prompt_list, int);
        if (-1 != arg)
        {
            printf(",%c", arg);
        }
    }

    printf("]? ");
    fflush(stdout);

    int res = getchar();
    arg = first;

    printf("\n");

    while (-1 != arg && res != arg)
    {
        arg = va_arg(choice_list, int);
    }

    if (-1 == arg)
    {
        return ERROR_TERMINAL_BAD_CHOICE;
    }

    *selection = arg;
    return STATUS_SUCCESS;
}
