#include <rcpr/model_assert.h>

#include <shadow/prop_valid_memory_range.h>
#include "../../../../src/secure_buffer/secure_buffer_internal.h"

RCPR_MODEL_STRUCT_TAG_GLOBAL_EXTERN(secure_buffer);

bool
prop_secure_buffer_valid(
    const secure_buffer* buffer)
{
    /* parameter sanity checks. */
    RCPR_MODEL_ASSERT(NULL != buffer);
    RCPR_MODEL_ASSERT_STRUCT_TAG_INITIALIZED(
        buffer->RCPR_MODEL_STRUCT_TAG_REF(secure_buffer), secure_buffer);

    return
            NULL != buffer->hdr.release
         && NULL != buffer->alloc
         && (
                NULL == buffer->data
             || prop_valid_memory_range(buffer->data, buffer->size));
}
