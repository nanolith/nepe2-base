/**
 * \file database/database_internal.h
 *
 * \brief Internal data structures and functions for interfacing with the
 * database.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <lmdb.h>
#include <nepe2/database.h>
#include <rcpr/resource/protected.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

struct database
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    MDB_env* env;
    MDB_dbi global_db;
    MDB_dbi metadata_db;
};

struct database_value
{
    RCPR_SYM(resource) hdr;
    RCPR_SYM(allocator)* alloc;
    const secure_buffer* encryption_key;
    const secure_buffer* database_key;
    secure_buffer* IV;
    secure_buffer* ciphertext;
    secure_buffer* mac;
};

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
