/**
 * \file metadata/metadata_empty_flag_get.c
 *
 * \brief Get the empty flag for the \ref metadata instance.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include "metadata_internal.h"

/**
 * \brief Get the empty flag for a given \ref metadata instance.
 *
 * \param meta              The metadata instance for this operation.
 *
 * \returns the empty flag.
 *
 * \pre
 *      - \p meta must reference a valid \ref metadata instance.
 * \post
 *      - returns the empty flag for this \ref metadata instance.
 */
bool
metadata_empty_flag_get(
    const metadata* meta)
{
    return
         !meta->hash_id
      || !meta->kdf_name
      || !meta->encoding
      || !meta->version_populated
      || !meta->creation_date_populated
      || !meta->revocation_date_populated
      || !meta->expiration_date_populated
      || !meta->password_length_populated
      || !meta->generation_populated
      || !meta->legacy_flag_populated;
}
