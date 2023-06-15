/**
 * \file secure_buffer/secure_buffer_data.c
 *
 * \brief Get the data pointer and size from a secure buffer instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "secure_buffer_internal.h"

/**
 * \brief Given a \ref secure_buffer instance, return the data pointer and size.
 *
 * \param size          Pointer to the size variable to receive the size.
 * \param buffer        The \ref secure_buffer instance to access.
 *
 * \returns the data pointer for this secure buffer.
 */
void*
secure_buffer_data(
    size_t* size, secure_buffer* buffer)
{
    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(prop_secure_buffer_valid(buffer));
    RCPR_MODEL_ASSERT(NULL != size);

    /* assign size. */
    *size = buffer->size;

    /* return the buffer data. */
    return buffer->data;
}
