#define CBMC_NO_MALLOC_OVERRIDE

#include <rcpr/allocator.h>
#include <stdlib.h>
#include <rcpr/model_assert.h>

RCPR_IMPORT_allocator;

status FN_DECL_MUST_CHECK
RCPR_SYM(allocator_allocate)(
    RCPR_SYM(allocator)* alloc, void** ptr, size_t size)
{
    RCPR_MODEL_ASSERT(prop_allocator_valid(alloc));
    RCPR_MODEL_ASSERT(NULL != ptr);

    void* tmp = malloc(size);
    if (NULL == tmp)
    {
        return ERROR_GENERAL_OUT_OF_MEMORY;
    }

    *ptr = tmp;
    return STATUS_SUCCESS;
}
