/**
 * \file test/metadata/test_metadata_create.cpp
 *
 * \brief Unit tests for metadata_create.
 */

#include <minunit/minunit.h>
#include <nepe2/metadata.h>

RCPR_IMPORT_allocator;
RCPR_IMPORT_resource;

TEST_SUITE(metadata_create);

/**
 * Verify that we can create a metadata instance and that it is empty.
 */
TEST(basics)
{
    allocator* alloc = nullptr;
    metadata* meta = nullptr;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* the metadata pointer is valid. */
    TEST_ASSERT(nullptr != meta);

    /* the metadata instance is empty. */
    TEST_EXPECT(metadata_empty_flag_get(meta));

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}
