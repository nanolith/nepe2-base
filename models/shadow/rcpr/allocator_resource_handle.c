#include "allocator_internal.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

RCPR_SYM(resource)*
RCPR_SYM(allocator_resource_handle)(
    RCPR_SYM(allocator)* alloc)
{
    return &alloc->hdr;
}
