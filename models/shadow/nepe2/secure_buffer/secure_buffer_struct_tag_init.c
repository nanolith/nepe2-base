#include <rcpr/model_assert.h>

#include "../../../../src/secure_buffer/secure_buffer_internal.h"

int RCPR_MODEL_STRUCT_TAG_GLOBAL_REF(secure_buffer);

void secure_buffer_struct_tag_init()
{
    RCPR_MODEL_STRUCT_TAG_GLOBAL_INIT(secure_buffer);
}
