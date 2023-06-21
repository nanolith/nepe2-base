#define CBMC_NO_MALLOC_OVERRIDE

#include <stdlib.h>

#include "allocator_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

static status malloc_allocator_release(resource* r);

status FN_DECL_MUST_CHECK
RCPR_SYM(malloc_allocator_create)(
    RCPR_SYM(allocator)** alloc)
{
    status retval;
    allocator* tmp = NULL;

    tmp = malloc(sizeof(allocator));
    if (NULL == tmp)
    {
        retval = ERROR_GENERAL_OUT_OF_MEMORY;
        goto done;
    }

    /* success. */
    tmp->hdr.release = &malloc_allocator_release;
    *alloc = tmp;
    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}

static status malloc_allocator_release(resource* r)
{
    free(r);

    return STATUS_SUCCESS;
}
