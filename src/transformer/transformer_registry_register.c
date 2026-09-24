/**
 * \file transformer/transformer_registry_register.c
 *
 * \brief Register a transformer instance.
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
 * \brief Register a transformer with the registry.
 *
 * \note The registry takes ownership of this transformer instance on success.
 *
 * \param xform         The \ref transformer instance to register.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_register(
    transformer* xform)
{
    status retval, release_retval;
    transformer_registry* reg;
    thread_mutex_lock* lock;
    resource* r;

    /* get the registry singleton. */
    retval = transformer_registry_singleton_get(&reg);
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
    retval = rbtree_find(&r, reg->dict, xform->name);
    if (STATUS_SUCCESS == retval)
    {
        retval = ERROR_TRANSFORMER_REGISTRY_NAME_ALREADY_REGISTERED;
        goto cleanup_lock;
    }

    /* insert this entry into the registry. */
    retval = rbtree_insert(reg->dict, &xform->hdr);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_lock;
    }

    /* success. */
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
