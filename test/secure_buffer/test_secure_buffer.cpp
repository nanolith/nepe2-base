/**
 * \file test/secure_buffer/test_secure_buffer.cpp
 *
 * \brief Unit tests for secure buffer.
 */

#include <minunit/minunit.h>
#include <nepe2/secure_buffer.h>
#include <rcpr/allocator.h>

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

TEST_SUITE(secure_buffer);

/**
 * Verify that we can create and release a secure buffer.
 */
TEST(basics)
{
    allocator* alloc = nullptr;
    secure_buffer* buffer = nullptr;
    uint8_t* ub = nullptr;
    size_t size = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a secure buffer. */
    TEST_ASSERT(STATUS_SUCCESS == secure_buffer_create(&buffer, alloc, 100));

    /* the buffer pointer is valid. */
    TEST_ASSERT(nullptr != buffer);

    /* we can get the buffer. */
    ub = (uint8_t*)secure_buffer_data(&size, buffer);

    /* the buffer is valid. */
    TEST_ASSERT(nullptr != ub);
    /* the size is correct. */
    TEST_ASSERT(100 == size);

    /* the buffer is initialized as zero. */
    for (size_t i = 0; i < size; ++i)
        TEST_ASSERT(0 == ub[i]);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == resource_release(secure_buffer_resource_handle(buffer)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}
