/**
 * \file base/nepe2_register.c
 *
 * \brief Register all extensible mechanisms.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <nepe2/nepe2.h>

/**
 * \brief Register all implementations of mappers, transformers, and other
 * extensible mechanisms.
 */
status FN_DECL_MUST_CHECK nepe2_register()
{
    status retval;

    retval = mappers_register();
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    retval = transformers_register();
    if (STATUS_SUCCESS != retval)
    {
        goto done;
    }

    retval = STATUS_SUCCESS;
    goto done;

done:
    return retval;
}
