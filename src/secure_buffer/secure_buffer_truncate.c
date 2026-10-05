/**
 * \file secure_buffer/secure_buffer_truncate.c
 *
 * \brief Truncate a secure buffer.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "secure_buffer_internal.h"

/**
 * \brief Truncate a \ref secure_buffer instance to the given size.
 *
 * \note if this size is larger than the buffer size, this operation does
 * nothing.
 *
 * \param buffer        The buffer to truncate.
 * \param size          The new truncated size.
 */
void secure_buffer_truncate(secure_buffer* buffer, size_t size)
{
    size_t original_size = buffer->size;

    /* make sure this is a truncation... */
    if (size >= original_size)
    {
        return;
    }

    /* make sure the truncated area has been cleared. */
    uint8_t* bptr = buffer->data;
    bptr += size;
    explicit_bzero(bptr, size - original_size);

    /* update the buffer size. */
    buffer->size = size;
}
