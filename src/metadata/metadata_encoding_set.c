/**
 * \file metadata/metadata_encoding_set.c
 *
 * \brief Set the encoding field for a \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Set the encoding for a given \ref metadata instance.
 *
 * \param meta          The metadata instance for this operation.
 * \param encoding      The name of the encoding for this instance.
 *
 * \note If this \ref metadata instance is currently empty, and if this is the
 * last field to set in order to make it whole, then this setter will make the
 * instance whole. This setter copies the encoding to an internal secure buffer.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 *      - \p encoding must be a valid pointer pointing to a valid character
 *        string.
 * \post
 *      - On success, the \p encoding field for this \ref metadata instance is
 *        set to a copy of the data provided.
 *      - On failure, \p meta is unchanged.
 */
status FN_DECL_MUST_CHECK
metadata_encoding_set(
    metadata* meta, const char* encoding)
{
    status retval;
    bool symbolic = false;
    size_t encoding_length = strlen(encoding);
    secure_buffer* tmp = NULL;
    void* data = NULL;
    size_t data_size = 0U;

    /* is this a symbolic encoding? */
    if (encoding_length >= 9 && !memcmp(encoding, "SYMBOLIC-", 9))
    {
        symbolic = true;
    }
    else
    {
        switch (encoding_length)
        {
            /* binary encoding xlat tables are supported. */
            case 2:
            /* base-4 encoding xlat tables are supported. */
            case 4:
            /* octal encoding xlat tables are supported. */
            case 8:
            /* hex encoding xlat tables are supported. */
            case 16:
            /* base-32 encoding xlat tables are supported. */
            case 32:
            /* base-64 encoding xlat tables are supported. */
            case 64:
            /* base-128 encoding xlat tables are supported. */
            case 128:
                break;

            default:
                retval = ERROR_METADATA_BAD_ENCODING_LENGTH;
                goto done;
        }
    }

    /* copy the encoding value. */
    retval = secure_buffer_create(&tmp, meta->alloc, encoding_length + 1);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the secure buffer data. */
    data = secure_buffer_data(&data_size, tmp);

    /* clear the data field and copy the encoding. */
    memset(data, 0, encoding_length + 1);
    memcpy(data, encoding, encoding_length);

    /* success: set the field. */
    meta->symbolic_encoding = symbolic;
    meta->encoding = tmp;
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
