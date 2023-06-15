/**
 * \file secure_buffer/secure_buffer_create.c
 *
 * \brief Create a secure buffer instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <rcpr/model_assert.h>
#include <string.h>

#include "secure_buffer_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Create a secure buffer of the given size using the given allocator.
 *
 * \param buffer        Pointer to the pointer to receive the secure buffer on
 *                      success.
 * \param alloc         The allocator instance to use for this operation.
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
secure_buffer_create(
    secure_buffer** buffer, RCPR_SYM(allocator)* alloc, size_t size)
{
    status retval, release_retval;
    secure_buffer* tmp = NULL;

    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(NULL != buffer);
    RCPR_MODEL_ASSERT(prop_allocator_valid(alloc));
    RCPR_MODEL_ASSERT(size > 0);

    /* allocate memory for the buffer struct. */
    retval = allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* clear out the structure. */
    memset(tmp, 0, sizeof(*tmp));

    /* the tag is not set by default. */
    RCPR_MODEL_ASSERT_STRUCT_TAG_NOT_INITIALIZED(
        tmp->RCPR_MODEL_STRUCT_TAG_REF(secure_buffer), secure_buffer);

    /* set the tag. */
    RCPR_MODEL_STRUCT_TAG_INIT(
        tmp->RCPR_MODEL_STRUCT_TAG_REF(secure_buffer), secure_buffer);

    /* initialize resource. */
    resource_init(&tmp->hdr, &secure_buffer_resource_release);
    tmp->alloc = alloc;
    tmp->size = size;

    /* allocate buffer memory. */
    retval = allocator_allocate(alloc, &tmp->data, size);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* clear buffer memory. */
    memset(tmp->data, 0, size);

    /* verify that this secure buffer is now valid. */
    RCPR_MODEL_ASSERT(prop_secure_buffer_valid(tmp));

    /* success. */
    *buffer = tmp;
    retval = STATUS_SUCCESS;
    goto done;

cleanup_tmp:
    release_retval = resource_release(&tmp->hdr);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
