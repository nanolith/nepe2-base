/**
 * \file secure_buffer/secure_buffer_create_from_base64.c
 *
 * \brief Create a secure buffer instance from a Base64 input string.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>
#include <openssl/evp.h>

#include "secure_buffer_internal.h"

RCPR_IMPORT_resource;

/**
 * \brief Create a secure buffer from a Base64 string.
 *
 * \param buffer        Pointer to the pointer to receive the secure buffer on
 *                      success.
 * \param alloc         The allocator instance to use for this operation.
 * \param input         Pointer to the input buffer in Base64 representation.
 * \param size          The size of the secure buffer to allocate.
 *
 * \note This secure buffer is a \ref resource that must be released by calling
 * \ref resource_release on its resource handle when it is no longer needed by
 * the caller. The resource handle can be accessed by calling
 * \ref secure_buffer_resource_handle on this secure buffer instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p buffer must not reference a valid \ref secure_buffer instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 * \post
 *      - On success, \p buffer is set to a pointer to a valid
 *        \ref secure_buffer instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p buffer is not changed and an error status is returned.
 */
status FN_DECL_MUST_CHECK
secure_buffer_create_from_base64(
    secure_buffer** buffer, RCPR_SYM(allocator)* alloc, const void* input,
    size_t size)
{
    status retval, release_retval;
    secure_buffer* tmp = NULL;
    EVP_ENCODE_CTX *ctx;
    const size_t output_size = (size * 3) / 4;
    int output_bytes = 0, final_bytes = 0;
    uint8_t *tmp_data;
    size_t tmp_size;

    /* create an output buffer to hold the data. */
    retval = secure_buffer_create(&tmp, alloc, output_size);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* get buffer data. */
    tmp_data = secure_buffer_data(&tmp_size, tmp);

    /* create the encoder context and initialize for decoding. */
    ctx = EVP_ENCODE_CTX_new();
    if (NULL == ctx)
    {
        retval = ERROR_SECURE_BUFFER_BASE64_DECODE;
        goto cleanup_tmp;
    }
    EVP_DecodeInit(ctx);

    /* decode data. */
    retval =
        EVP_DecodeUpdate(
            ctx, tmp_data, &output_bytes, (const uint8_t*)input, (int)size);
    if (retval < 0)
    {
        retval = ERROR_SECURE_BUFFER_BASE64_DECODE;
        goto cleanup_ctx;
    }

    /* finalize data. */
    retval = EVP_DecodeFinal(ctx, tmp_data + output_bytes, &final_bytes);
    if (retval < 0)
    {
        retval = ERROR_SECURE_BUFFER_BASE64_DECODE;
        goto cleanup_ctx;
    }

    /* set buffer size. */
    secure_buffer_truncate(tmp, output_bytes + final_bytes);

    /* success. */
    retval = STATUS_SUCCESS;
    *buffer = tmp;
    tmp = NULL;
    goto cleanup_ctx;

cleanup_ctx:
    EVP_ENCODE_CTX_free(ctx);

cleanup_tmp:
    if (NULL != tmp)
    {
        release_retval = resource_release(secure_buffer_resource_handle(tmp));
        if (STATUS_SUCCESS != release_retval)
        {
            retval = release_retval;
        }
    }

done:
    return retval;
}
