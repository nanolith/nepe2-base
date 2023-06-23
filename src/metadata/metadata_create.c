/**
 * \file metadata/metadata_create.c
 *
 * \brief Create a metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <string.h>

#include "metadata_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

/**
 * \brief Create an empty metadata instance using the given allocator.
 *
 * \param meta          Pointer to the metadata instance.
 * \param alloc         The allocator instance to use for this operation.
 *
 * \note This metadata instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * metadata_resource_handle on this metadata instance.
 *
 * This metadata instance is empty. Empty instances cannot be serialized or
 * accessed. They are empty vessels for serialization.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - ERROR_GENERAL_OUT_OF_MEMORY if this method failed due to an
 *        out-of-memory condition.
 *
 * \pre
 *      - \p meta must not reference a valid \ref metadata instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 * \post
 *      - On success, \p meta is set to a pointer to a valid
 *        \ref metadata instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p meta is not changed and an error status is returned.
 */
status FN_DECL_MUST_CHECK
metadata_create(
    metadata** meta, RCPR_SYM(allocator)* alloc)
{
    status retval;
    metadata* tmp = NULL;

    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(NULL != meta);
    RCPR_MODEL_ASSERT(prop_allocator_valid(alloc));

    /* allocate memory for the metadata instance. */
    retval = allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* clear memory. */
    RCPR_MODEL_EXEMPT(memset(tmp, 0, sizeof(*tmp)));

    /* the tag is not set by default. */
    RCPR_MODEL_ONLY(tmp->RCPR_MODEL_STRUCT_TAG_REF(metadata) = 0);
    RCPR_MODEL_ASSERT_STRUCT_TAG_NOT_INITIALIZED(
        tmp->RCPR_MODEL_STRUCT_TAG_REF(metadata), metadata);

    /* set the tag. */
    RCPR_MODEL_STRUCT_TAG_INIT(
        tmp->RCPR_MODEL_STRUCT_TAG_REF(metadata), metadata);

    /* initialize resource. */
    resource_init(&tmp->hdr, &metadata_resource_release);
    tmp->alloc = alloc;

    /* verify that this metadata instance is now valid. */
    RCPR_MODEL_ASSERT(prop_metadata_valid(tmp));

    /* success. */
    *meta = tmp;
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
