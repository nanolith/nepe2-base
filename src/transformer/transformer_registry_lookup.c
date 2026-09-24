/**
 * \file transformer/transformer_registry_lookup.c
 *
 * \brief Look up a transformer instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/error_codes.h>

#include "transformer_internal.h"

RCPR_IMPORT_rbtree;
RCPR_IMPORT_resource;
RCPR_IMPORT_thread;

/**
 * \brief Look up a transformer by name from the registry.
 *
 * \note This transformer is owned by the registry.
 *
 * \param xform         Pointer to the \ref transformer pointer to be set with
 *                      the transformer if found.
 * \param name          The name of the transformer to look up.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_lookup(
    transformer** xform, const char* name)
{
    status retval, release_retval;
    transformer_registry* reg;
    transformer* tmp;
    thread_mutex_lock* lock;

    /* get the singleton registry. */
    retval = transformer_registry_singleton_get(&reg);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* acquire the mutex for this registry. */
    retval = thread_mutex_lock_acquire(&lock, reg->mutex);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* look up the transformer by name. */
    retval = rbtree_find((resource**)&tmp, reg->dict, name);
    if (STATUS_SUCCESS != retval)
    {
        retval = ERROR_TRANSFORMER_REGISTRY_NOT_FOUND;
        goto cleanup_lock;
    }

    /* success. */
    *xform = tmp;
    retval = STATUS_SUCCESS;
    goto cleanup_lock;

cleanup_lock:
    release_retval = resource_release(thread_mutex_lock_resource_handle(lock));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

done:
    return retval;
}
