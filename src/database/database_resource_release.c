/**
 * \file database/database_resource_release.c
 *
 * \brief Release a \ref database instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "database_internal.h"

RCPR_IMPORT_allocator_as(rcpr);

/**
 * \brief Release a \ref database instance.
 *
 * \param r             The \ref database \ref resource to release.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status database_resource_release(RCPR_SYM(resource)* r)
{
    status retval = STATUS_SUCCESS, release_retval;
    database* db = (database*)r;

    /* cache allocator. */
    rcpr_allocator* alloc = db->alloc;

    if (NULL != db->env)
    {
        /* sync the database to disk. */
        mdb_env_sync(db->env, 1);

        /* close database handles. */
        mdb_dbi_close(db->env, db->global_db);
        mdb_dbi_close(db->env, db->metadata_db);

        /* close the environment. */
        mdb_env_close(db->env);
    }

    /* clear memory and reclaim the structure. */
    explicit_bzero(db, sizeof(*db));
    release_retval = rcpr_allocator_reclaim(alloc, db);
    if (STATUS_SUCCESS != release_retval)
    {
        retval = release_retval;
    }

    /* decode result. */
    return retval;
}
