/**
 * \file mapper/mapper_create.c
 *
 * \brief Create a mapper instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <rcpr/string.h>
#include <rcpr/vtable.h>

#include "mapper_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_resource;
RCPR_IMPORT_string_as(rcpr);

RCPR_VTABLE resource_vtable mapper_vtable = {
    .release = &mapper_resource_release
};

/**
 * \brief Create a mapper instance with the given name and mapping function.
 *
 * \param m             Pointer to the mapper pointer to be set to the
 *                      created mapper instance on success.
 * \param alloc         The allocator to use for this operation.
 * \param name          The name of this mapper.
 * \param map_fn        The mapper function to use for this instance.
 *
 * \note This mapper instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * mapper_resource_handle on this mapper instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 *
 * \pre
 *      - \p m must not reference a valid \ref mapper instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 *      - \p name must be a valid string and must not be NULL.
 *      - \p map_fn must be a valid function and must not be NULL.
 * \post
 *      - On success, \p m is set to a pointer to a valid \ref mapper instance,
 *        which is a \ref resource owned by the caller that must be released
 *        when no longer needed.
 *      - On failure, \p m is set to NULL and an error status is returned.
 */
status FN_DECL_MUST_CHECK
mapper_create(
    mapper** m, RCPR_SYM(allocator)* alloc, const char* name, mapper_fn map_fn)
{
    status retval, release_retval;
    mapper* tmp;

    /* allocate memory for this instance. */
    retval = rcpr_allocator_allocate(alloc, (void**)&tmp, sizeof(*tmp));
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* initialize resource. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &mapper_vtable);
    tmp->alloc = alloc;
    tmp->map_fn = map_fn;

    /* copy name. */
    retval = rcpr_strdup(&tmp->name, alloc, name);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* success. */
    *m = tmp;
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
