/**
 * \file test/metadata/test_metadata_create.cpp
 *
 * \brief Unit tests for metadata_create.
 */

#include <minunit/minunit.h>
#include <nepe2/error_codes.h>
#include <nepe2/metadata.h>
#include <string.h>

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

/**
 * We can set and get a hash id.
 */
TEST(metadata_hash_id_set_get)
{
    const uint8_t hash_id[] = {
        0x5e, 0x5f, 0x4e, 0xfb, 0x2c, 0xd4, 0x4c, 0x21,
        0x9b, 0x33, 0x05, 0xda, 0x5d, 0xb7, 0xd5, 0x65,
        0x03, 0xeb, 0xc4, 0xe4, 0x5b, 0x95, 0x49, 0x12,
        0xa2, 0x5f, 0x5f, 0x97, 0xc8, 0xf3, 0x03, 0x81 };
    const size_t hash_id_size = sizeof(hash_id);
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    const void* hptr = nullptr;
    size_t hptr_size = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the hash id before it is set, we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_hash_id_get(&hptr, &hptr_size, meta));

    /* set the hash id. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == metadata_hash_id_set(meta, hash_id, hash_id_size));

    /* we can now get the hash id. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == metadata_hash_id_get(&hptr, &hptr_size, meta));

    /* the hash pointer is set by the getter. */
    TEST_ASSERT(nullptr != hptr);

    /* the size matches. */
    TEST_ASSERT(hash_id_size == hptr_size);

    /* the data matches. */
    TEST_EXPECT(!memcmp(hptr, hash_id, hash_id_size));

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set a hash id from a secure buffer and get it.
 */
TEST(metadata_hash_id_set_from_secure_buffer_get)
{
    const uint8_t hash_id[] = {
        0x5e, 0x5f, 0x4e, 0xfb, 0x2c, 0xd4, 0x4c, 0x21,
        0x9b, 0x33, 0x05, 0xda, 0x5d, 0xb7, 0xd5, 0x65,
        0x03, 0xeb, 0xc4, 0xe4, 0x5b, 0x95, 0x49, 0x12,
        0xa2, 0x5f, 0x5f, 0x97, 0xc8, 0xf3, 0x03, 0x81 };
    const size_t hash_id_size = sizeof(hash_id);
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    secure_buffer* buffer = nullptr;
    const void* hptr = nullptr;
    size_t hptr_size = 0U;
    void* buffer_data = nullptr;
    size_t buffer_size = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* we can successfully create a securebuffer instance. */
    TEST_ASSERT(
        STATUS_SUCCESS == secure_buffer_create(&buffer, alloc, hash_id_size));

    /* get the buffer pointer and size. */
    buffer_data = secure_buffer_data(&buffer_size, buffer);

    /* copy the hash id to the buffer. */
    memcpy(buffer_data, hash_id, hash_id_size);

    /* if we attempt to get the hash id before it is set, we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_hash_id_get(&hptr, &hptr_size, meta));

    /* set the hash id from our buffer. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == metadata_hash_id_set_from_secure_buffer(meta, buffer));

    /* we can now get the hash id. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == metadata_hash_id_get(&hptr, &hptr_size, meta));

    /* the hash pointer is set by the getter. */
    TEST_ASSERT(nullptr != hptr);

    /* the size matches. */
    TEST_ASSERT(hash_id_size == hptr_size);

    /* the data matches. */
    TEST_EXPECT(!memcmp(hptr, hash_id, hash_id_size));

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == resource_release(secure_buffer_resource_handle(buffer)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the version.
 */
TEST(metadata_version_set_get)
{
    const uint32_t VERSION = 0x12345;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint32_t version = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the version before it is set, we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET == metadata_version_get(&version, meta));

    /* set the version. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_version_set(meta, VERSION));

    /* we can now get the version. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_version_get(&version, meta));

    /* the version matches. */
    TEST_EXPECT(VERSION == version);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the creation date.
 */
TEST(metadata_creation_date_set_get)
{
    const uint64_t CREATION_DATE = 0x54321;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint64_t creation_date = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the creation date before it is set,
     * we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_creation_date_get(&creation_date, meta));

    /* set the creation date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_creation_date_set(meta, CREATION_DATE));

    /* we can now get the creation date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_creation_date_get(&creation_date, meta));

    /* the creation date matches. */
    TEST_EXPECT(CREATION_DATE == creation_date);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the revocation date.
 */
TEST(metadata_revocation_date_set_get)
{
    const uint64_t REVOCATION_DATE = 0x7777777;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint64_t revocation_date = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the revocation date before it is set,
     * we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_revocation_date_get(&revocation_date, meta));

    /* set the revocation date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_revocation_date_set(meta, REVOCATION_DATE));

    /* we can now get the revocation date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_revocation_date_get(&revocation_date, meta));

    /* the revocation date matches. */
    TEST_EXPECT(REVOCATION_DATE == revocation_date);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the expiration date.
 */
TEST(metadata_expiration_date_set_get)
{
    const uint64_t EXPIRATION_DATE = 0x4444;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint64_t expiration_date = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the expiration date before it is set,
     * we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_expiration_date_get(&expiration_date, meta));

    /* set the expiration date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_expiration_date_set(meta, EXPIRATION_DATE));

    /* we can now get the expiration date. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_expiration_date_get(&expiration_date, meta));

    /* the expiration date matches. */
    TEST_EXPECT(EXPIRATION_DATE == expiration_date);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the password length.
 */
TEST(metadata_password_length_set_get)
{
    const uint32_t PASSWORD_LENGTH = 16;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint32_t password_length = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the password length before it is set,
     * we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_password_length_get(&password_length, meta));

    /* set the password length. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_password_length_set(meta, PASSWORD_LENGTH));

    /* we can now get the password length. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_password_length_get(&password_length, meta));

    /* the password length matches. */
    TEST_EXPECT(PASSWORD_LENGTH == password_length);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the generation.
 */
TEST(metadata_generation_set_get)
{
    const uint32_t GENERATION = 7;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    uint32_t generation = 0U;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the generation before it is set, we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_generation_get(&generation, meta));

    /* set the generation. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_generation_set(meta, GENERATION));

    /* we can now get the generation. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_generation_get(&generation, meta));

    /* the generation matches. */
    TEST_EXPECT(GENERATION == generation);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}

/**
 * We can set and get the legacy flag.
 */
TEST(metadata_legacy_flag_set_get)
{
    const bool LEGACY_FLAG = false;
    allocator* alloc = nullptr;
    metadata* meta = nullptr;
    bool legacy_flag = true;

    /* we can successfully create a malloc allocator. */
    TEST_ASSERT(STATUS_SUCCESS == malloc_allocator_create(&alloc));

    /* we can successfully create a metadata instance. */
    TEST_ASSERT(STATUS_SUCCESS == metadata_create(&meta, alloc));

    /* if we attempt to get the legacy flag before set, we get an error. */
    TEST_EXPECT(
        ERROR_METADATA_FIELD_NOT_SET
            == metadata_legacy_flag_get(&legacy_flag, meta));

    /* set the legacy flag. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_legacy_flag_set(meta, LEGACY_FLAG));

    /* we can now get the legacy flag. */
    TEST_ASSERT(
        STATUS_SUCCESS == metadata_legacy_flag_get(&legacy_flag, meta));

    /* the legacy flag matches. */
    TEST_EXPECT(LEGACY_FLAG == legacy_flag);

    /* clean up. */
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(metadata_resource_handle(meta)));
    TEST_ASSERT(
        STATUS_SUCCESS == resource_release(allocator_resource_handle(alloc)));
}
