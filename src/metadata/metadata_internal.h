/**
 * \file metadata/metadata_internal.h
 *
 * \brief Internal header for \ref metadata.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/metadata.h>
#include <rcpr/resource/protected.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct metadata
{
    RCPR_SYM(resource) hdr;
    RCPR_MODEL_STRUCT_TAG(metadata);
    RCPR_SYM(allocator)* alloc;
    secure_buffer* hash_id;
    secure_buffer* kdf_name;
    secure_buffer* encoding;
    bool symbolic_encoding;
    bool version_populated;
    uint32_t version;
    bool creation_date_populated;
    uint64_t creation_date;
    bool revocation_date_populated;
    uint64_t revocation_date;
    bool expiration_date_populated;
    uint64_t expiration_date;
    bool password_length_populated;
    uint32_t password_length;
    bool generation_populated;
    uint32_t generation;
    bool legacy_flag_populated;
    bool legacy_flag;
};

/**
 * \brief Release a \ref metadata resource.
 *
 * \param r             Pointer to the \ref metadata resource to be released.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - an error code on failure.
 */
status metadata_resource_release(RCPR_SYM(resource)* r);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
