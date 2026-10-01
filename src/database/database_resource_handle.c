/**
 * \file database/database_resource_handle.c
 *
 * \brief Return the \ref resource handle for a given \ref database instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "database_internal.h"

/**
 * \brief Given a \ref database instance, return the resource handle for this
 * \ref database instance.
 *
 * \param db            The \ref database instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref database instance.
 */
RCPR_SYM(resource)*
database_resource_handle(database* db)
{
    return &db->hdr;
}
