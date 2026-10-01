/**
 * \file database/database_value_resource_handle.c
 *
 * \brief Return the \ref resource handle for a given \ref database_value
 * instance.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "database_internal.h"

/**
 * \brief Given a \ref database_value instance, return the resource handle for
 * this \ref database_value instance.
 *
 * \param db            The \ref database_value instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref database instance.
 */
RCPR_SYM(resource)*
database_value_resource_handle(database_value* val)
{
    return &val->hdr;
}
