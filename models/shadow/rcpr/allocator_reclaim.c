#define CBMC_NO_MALLOC_OVERRIDE

#include <rcpr/allocator.h>
#include <stdlib.h>
#include <rcpr/model_assert.h>

RCPR_IMPORT_allocator;

status FN_DECL_MUST_CHECK
RCPR_SYM(allocator_reclaim)(
    RCPR_SYM(allocator)* alloc, void* ptr)
{
    RCPR_MODEL_ASSERT(prop_allocator_valid(alloc));
    RCPR_MODEL_ASSERT(NULL != ptr);

    free(ptr);

    return STATUS_SUCCESS;
}
