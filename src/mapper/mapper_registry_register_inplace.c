/**
 * \file mapper/mapper_registry_register_inplace.c
 *
 * \brief Register a mapper instance in place.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "mapper_internal.h"

RCPR_IMPORT_rbtree;
RCPR_IMPORT_resource;
RCPR_IMPORT_thread;

/**
 * \brief Register a mapper in place, using the registry's allocator.
 *
 * \param name          The name of this mapper.
 * \param map_fn        The mapper function to use for this mapper.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
mapper_registry_register_inplace(
    const char* name, mapper_fn map_fn)
{
    status retval, release_retval;
    mapper_registry* reg;
    thread_mutex_lock* lock;
    mapper* tmp;
    resource* r;

    /* get the registry singleton. */
    retval = mapper_registry_singleton_get(&reg);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* lock the registry mutex. */
    retval = thread_mutex_lock_acquire(&lock, reg->mutex);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* check to see if the entry already exists. */
    retval = rbtree_find(&r, reg->dict, name);
    if (STATUS_SUCCESS == retval)
    {
        retval = ERROR_MAPPER_REGISTRY_NAME_ALREADY_REGISTERED;
        goto cleanup_lock;
    }

    /* create the new instance. */
    retval = mapper_create(&tmp, reg->alloc, name, map_fn);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_lock;
    }

    /* insert this entry into the registry. */
    retval = rbtree_insert(reg->dict, &tmp->hdr);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_tmp;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    goto cleanup_lock;

cleanup_tmp:
    release_retval = resource_release(&tmp->hdr);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_lock:
    release_retval = resource_release(thread_mutex_lock_resource_handle(lock));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
