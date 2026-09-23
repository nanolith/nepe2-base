/**
 * \file transformer/transformer_create.c
 *
 * \brief Create a transformer instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <rcpr/string.h>
#include <rcpr/vtable.h>

#include "transformer_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;
RCPR_IMPORT_string_as(rcpr);

RCPR_VTABLE resource_vtable transformer_vtable = {
    .release = &transformer_resource_release
};

/**
 * \brief Create a transformer instance with the given name and transformer
 * function.
 *
 * \param xform         Pointer to the transformer pointer to be set to the
 *                      created transformer instance on success.
 * \param alloc         The allocator to use for this operation.
 * \param name          The name of this transformer.
 * \param xform_fn      The transformer function to use for this instance.
 *
 * \note This transformer instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * transformer_resource_handle on this transformer instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 *
 * \pre
 *      - \p xform must not reference a valid \ref transformer instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 *      - \p name must be a valid string and must not be NULL.
 *      - \p xform_fn must be a valid function and must not be NULL.
 * \post
 *      - On success, \p xform is set to a pointer to a valid
 *        \ref transformer instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p xform is set to NULL and an error status is returned.
 */
status FN_DECL_MUST_CHECK
transformer_create(
    transformer** xform, RCPR_SYM(allocator)* alloc, const char* name,
    transformer_fn xform_fn)
{
    status retval, release_retval;
    transformer* tmp;

    /* allocate memory for this instance. */
    retval = rcpr_allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize resource. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &transformer_vtable);
    tmp->alloc = alloc;
    tmp->xform_fn = xform_fn;

    /* copy name. */
    retval = rcpr_strdup(&tmp->name, alloc, name);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* success. */
    *xform = tmp;
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
