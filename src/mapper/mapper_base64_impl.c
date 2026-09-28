/**
 * \file mapper/mapper_base64_impl.c
 *
 * \brief Implementation of the base64 mapper.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <openssl/evp.h>

#include "mapper_internal.h"

MAPPER_REGISTER("SYMBOLIC-Base64", &base64_mapper_function);

/**
 * \brief A mapper that performs a base64 mapping operation of a raw password to
 * a mapped password.
 *
 * \param mapped        Pointer to the secure buffer pointer to be set to the
 *                      mapped password on success.
 * \param alloc         The allocator to use for this operation.
 * \param raw           The raw password to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status base64_mapper_function(
    secure_buffer** mapped, RCPR_SYM(allocator)* alloc,
    const secure_buffer* raw)
{
    status retval;
    secure_buffer* tmp;
    const char *raw_data, *tmp_data;
    size_t raw_size, tmp_size;

    /* get the raw buffer data. */
    raw_data = secure_buffer_data(&raw_size, (secure_buffer*)raw);

    /* create output buffer. */
    retval = secure_buffer_create(&tmp, alloc, EVP_ENCODE_LENGTH(raw_size));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get the temporary buffer data. */
    tmp_data = secure_buffer_data(&tmp_size, tmp);

    /* Base64 encode the raw password. */
    int encoded_size =
        EVP_EncodeBlock(
            (unsigned char*)tmp_data, (const unsigned char*)raw_data, raw_size);

    /* truncate the secure buffer to the encoded size. */
    secure_buffer_truncate(tmp, encoded_size);

    /* Success. */
    retval = STATUS_SUCCESS;
    *mapped = tmp;
    goto done;

done:
    return retval;
}
