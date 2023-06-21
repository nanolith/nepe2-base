#include <rcpr/allocator.h>

bool
RCPR_SYM(prop_allocator_valid)(
    const RCPR_SYM(allocator)* alloc)
{
    return NULL != alloc;
}
