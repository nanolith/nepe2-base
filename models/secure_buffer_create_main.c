#include <nepe2/secure_buffer.h>
#include <rcpr/model_assert.h>

#include "shadow/prop_valid_memory_range.h"

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

void secure_buffer_struct_tag_init();

uint8_t nondet_size();

int main(int argc, char* argv[])
{
    allocator* alloc = NULL;
    secure_buffer* buffer = NULL;
    status retval, release_retval;
    size_t SECURE_BUFFER_SIZE = ((size_t)nondet_size()) + 1;
    uint8_t* ub = NULL;
    size_t size = 0U;

    secure_buffer_struct_tag_init();

    /* create a malloc allocator. */
    retval = malloc_allocator_create(&alloc);
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    /* create a secure buffer instance. */
    retval = secure_buffer_create(&buffer, alloc, SECURE_BUFFER_SIZE);
    if (STATUS_SUCCESS != retval)
    {
        goto cleanup_alloc;
    }

    /* get the buffer data. */
    ub = secure_buffer_data(&size, buffer);

    /* it is not NULL. */
    RCPR_MODEL_ASSERT(NULL != ub);
    /* the size is correct. */
    RCPR_MODEL_ASSERT(SECURE_BUFFER_SIZE == size);
    /* it is visible. */
    RCPR_MODEL_ASSERT(prop_valid_memory_range(ub, SECURE_BUFFER_SIZE));

    goto cleanup_buffer;

cleanup_buffer:
    release_retval = resource_release(secure_buffer_resource_handle(buffer));
    RCPR_MODEL_ASSERT(STATUS_SUCCESS == release_retval);

cleanup_alloc:
    release_retval = resource_release(allocator_resource_handle(alloc));
    RCPR_MODEL_ASSERT(STATUS_SUCCESS == release_retval);

done:
    return retval;
}
