/**
 * \file terminal/terminal_readpassphrase.h
 *
 * \brief Read a passphrase from the terminal without echoing.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <nepe2/terminal.h>
#include <signal.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

RCPR_IMPORT_resource;

typedef struct terminal_savestate terminal_savestate;
struct terminal_savestate
{
    struct sigaction saved_handlers[32];
    struct sigaction installed_handlers[32];
};

/* globals. */
static struct termios saved_attrs;
static bool exit_loop_error;

static status terminal_set_password_mode(terminal_savestate*);
static status terminal_clear_password_mode(terminal_savestate*);
static void terminal_sig_handler(int);

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
    bool truncate)
{
    status retval, release_retval;
    secure_buffer* tmp;
    terminal_savestate savestate;
    uint8_t* tmp_data;
    size_t tmp_size;
    char ch = 0;
    bool warn_truncated = false;

    /* create a secure buffer for holding this passphrase. */
    retval = secure_buffer_create(&tmp, alloc, max_length);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* Hook the terminal and set the flags. */
    retval = terminal_set_password_mode(&savestate);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* start the loop. */
    tmp_data = (uint8_t*)secure_buffer_data(&tmp_size, tmp);
    size_t offset = 0;
    exit_loop_error = false;
    while (!exit_loop_error && '\n' != ch)
    {
        if (read(0, &ch, 1) < 0)
        {
            exit_loop_error = true;
        }

        /* append the character if it is not enter. */
        if ('\n' != ch)
        {
            /* we will skip over characters if truncate is enabled. */
            if (truncate && tmp_size == offset)
            {
                warn_truncated = true;
                continue;
            }
            
            tmp_data[offset++] = ch;
            if (offset >= tmp_size)
            {
                fprintf(stderr, "passphrase size too large.\n");
                exit_loop_error = true;
            }
        }
    }

    /* ensure that we can synchronize with stdin. */
    tcflush(0, TCIFLUSH);

    /* Warn if the passphrase was truncated. */
    if (warn_truncated)
    {
        fprintf(
            stderr, "WARNING: passphrase truncated at %zu characters.\n",
            tmp_size);
    }

    /* Remove hook and restore terminal flags. */
    retval = terminal_clear_password_mode(&savestate);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* Did an error occur? */
    if (exit_loop_error)
    {
        retval = ERROR_TERMINAL_READPASSPHRASE;
        goto cleanup_tmp;
    }

    /* truncate the passphrase buffer to the read size. */
    secure_buffer_truncate(tmp, offset);

    /* success. */
    printf("\n");
    retval = STATUS_SUCCESS;
    *passphrase = tmp;
    goto done;

cleanup_tmp:
    release_retval = resource_release(secure_buffer_resource_handle(tmp));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}

/**
 * \brief This signal handler restores terminal attributes and sets the
 * exit_loop_error flag.
 *
 * \param sig           The signal that triggered this handler.
 */
static void terminal_sig_handler(int sig)
{
    (void)sig;

    exit_loop_error = true;

    tcsetattr(0, TCSANOW, &saved_attrs);
}

/**
 * \brief Set password entry mode on this terminal by turning off echo and
 * installing signal handlers to ensure that this mode will be cleared before
 * the process is terminated.
 *
 * \param savestate             Context for saving signal handler state.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status terminal_set_password_mode(terminal_savestate* savestate)
{
    status retval;
    struct termios attrs;

    /* get the current terminal flags. */
    if (tcgetattr(0, &attrs) < 0)
    {
        retval = ERROR_TERMINAL_STDIN_NOT_A_TERMINAL;
        goto done;
    }

    /* save these attributes. */
    memcpy(&saved_attrs, &attrs, sizeof(attrs));

    /* clear the saved state. */
    explicit_bzero(savestate, sizeof(*savestate));

    /* iterate through the 32 possible signal handlers, saving their state and
     * installing new handlers. */
    for (int i = 0; i < 32; ++i)
    {
        savestate->installed_handlers[i].sa_handler = &terminal_sig_handler;
        sigaction(
            i+1, &savestate->installed_handlers[i],
            &savestate->saved_handlers[i]);
    }

    /* disable the canonical and echo flags. */
    attrs.c_lflag &= ~ICANON;
    attrs.c_lflag &= ~ECHO;

    /* read should be be one character at a time and it should block. */
    attrs.c_cc[VMIN] = 1;
    attrs.c_cc[VTIME] = 0;

    /* set the terminal attributes. */
    if (tcsetattr(0, TCSANOW, &attrs) < 0)
    {
        retval = ERROR_TERMINAL_TCSETATTR;
        goto done;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}

/**
 * \brief Clear password entry mode on this terminal by restoring the terminal
 * flags and signal handlers.
 *
 * \param savestate             Context for restoring signal handler state.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status terminal_clear_password_mode(terminal_savestate* savestate)
{
    status retval;

    /* restore terminal attributes. */
    if (tcsetattr(0, TCSANOW, &saved_attrs) < 0)
    {
        retval = ERROR_TERMINAL_TCSETATTR;
        goto done;
    }

    /* Restore each of the signal handlers. */
    for (int i = 0; i < 32; ++i)
    {
        sigaction(i+1, &savestate->saved_handlers[i], NULL);
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
