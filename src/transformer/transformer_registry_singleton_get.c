/**
 * \file transformer/transformer_registry_singleton_get.c
 *
 * \brief Get the singleton transformer registry.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <rcpr/vtable.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "transformer_internal.h"

RCPR_IMPORT_allocator_as(rcpr);
RCPR_IMPORT_rbtree;
RCPR_IMPORT_resource;
RCPR_IMPORT_thread;

/* forward decls. */
static status transformer_registry_create(transformer_registry** reg);
static status transformer_registry_release(transformer_registry* reg);

/* globals. */
static _Atomic(transformer_registry*) registry = NULL;

RCPR_VTABLE resource_vtable transformer_registry_vtable = {
    .release = &transformer_resource_release
};

/**
 * \brief Get the singleton transformer registry implementation.
 *
 * \param reg           Pointer to the \ref transformer_registry pointer to be
 *                      set with the singleton instance on success.
 *
 * \note This method optionally allocates a registry instance. This instance
 * exists as a global. This allocation occurs only once, on a single thread. All
 * other threads will receive this single copy.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_singleton_get(
    transformer_registry** reg)
{
    status retval;
    transformer_registry* tmp =
        atomic_load_explicit(&registry, memory_order_acquire);

    /* the singleton hasn't been allocated yet. */
    if (NULL == tmp)
    {
        /* create a registry instance. */
        retval = transformer_registry_create(&tmp);
        if (STATUS_SUCCESS != retval)
        {
            goto done;
        }

        /* attempt to set this instance as the singleton. */
        transformer_registry* expected = NULL;
        if (atomic_compare_exchange_strong(&registry, &expected, tmp))
        {
            /* we were the first thread to do so. */
            *reg = tmp;
            retval = STATUS_SUCCESS;
            goto done;
        }
        else
        {
            /* another thread beat us to it, so release our created instance. */
            retval = transformer_registry_release(tmp);
            if (STATUS_SUCCESS != retval)
            {
                goto done;
            }

            /* use the instance created by the other thread. */
            *reg = expected;
            retval = STATUS_SUCCESS;
            goto done;
        }
    }
    /* the singleton has already been allocated. */
    else
    {
        *reg = tmp;
        retval = STATUS_SUCCESS;
        goto done;
    }

done:
    return retval;
}

/**
 * \brief Create a transformer registry instance.
 *
 * \param reg               Pointer to the transformer registry pointer to set
 *                          with the created registry on success.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status transformer_registry_create(transformer_registry** reg)
{
    status retval, release_retval;
    transformer_registry* tmp;

    /* allocate memory for this instance. */
    tmp = (transformer_registry*)malloc(sizeof(*tmp));
    if (NULL == tmp)
    {
        retval = ERROR_GENERAL_OUT_OF_MEMORY;
        goto done;
    }

    /* initialize resource. */
    explicit_bzero(tmp, sizeof(*tmp));
    resource_init(&tmp->hdr, &transformer_registry_vtable);

    /* create malloc allocator. */
    retval = rcpr_malloc_allocator_create(&tmp->alloc);
    if (STATUS_SUCCESS != retval)
    {
        goto free_tmp;
    }

    /* create dictionary instance. */
    retval =
        rbtree_create(
            &tmp->dict, tmp->alloc, &transformer_dict_compare,
            &transformer_dict_key, NULL);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_alloc;
    }

    /* create mutex instance. */
    retval = thread_mutex_create(&tmp->mutex, tmp->alloc);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_dict;
    }

    /* success. */
    retval = STATUS_SUCCESS;
    *reg = tmp;
    goto done;

cleanup_dict:
    release_retval = resource_release(rbtree_resource_handle(tmp->dict));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

cleanup_alloc:
    release_retval =
        resource_release(rcpr_allocator_resource_handle(tmp->alloc));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

free_tmp:
    explicit_bzero(tmp, sizeof(*tmp));
    free(tmp);

done:
    return retval;
}

/**
 * \brief Release a registry instance that failed compare-and-exchange.
 *
 * \param reg           The instance to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
static status transformer_registry_release(transformer_registry* reg)
{
    status retval = STATUS_SUCCESS, release_retval;

    /* clean up mutex. */
    release_retval = resource_release(thread_mutex_resource_handle(reg->mutex));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* clean up dict. */
    release_retval = resource_release(rbtree_resource_handle(reg->dict));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* clean up allocator. */
    release_retval =
        resource_release(rcpr_allocator_resource_handle(reg->alloc));
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* clean up instance. */
    explicit_bzero(reg, sizeof(*reg));
    free(reg);

    /* decode result. */
    return retval;
}
