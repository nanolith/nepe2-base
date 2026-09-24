/**
 * \file nepe2/transformer.h
 *
 * \brief The transformer interface provides a means of turning a master
 * passphrase, a session passphrase, and metadata into a secure buffer that
 * contains raw password information.
 *
 * The transformer corresponds roughly to the KDF algorithm defined by the
 * metadata.
 *
 * \copyright 2026 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <nepe2/metadata.h>
#include <nepe2/secure_buffer.h>
#include <rcpr/allocator.h>
#include <rcpr/resource.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief The transformer is an interface for transforming passphrases into
 * passwords.
 */
typedef struct transformer transformer;

/**
 * \brief The transformer registry allows transformers to be registered and
 * queried by name.
 */
typedef struct transformer_registry transformer_registry;

/**
 * \brief A transformer function takes an allocator, a master passphrase, a
 * session passphrase, and metadata to produce a secure buffer containing a
 * raw generated password.
 *
 * \param raw           Pointer to the secure buffer pointer to be set to the
 *                      raw generated password on success.
 * \param alloc         The allocator to use for this operation.
 * \param master        The master passphrase to use for this operation.
 * \param session       The session passphrase to use for this operation.
 * \param meta          The metadata to use for this operation.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
typedef status (*transformer_fn)(
    secure_buffer** raw, RCPR_SYM(allocator)* alloc,
    const secure_buffer* master, const secure_buffer* session,
    const metadata* meta);

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Create a transformer instance with the given name and transformer
 * function.
 *
 * \param xform         Pointer to the transformer pointer to be set to the
 *                      created transformer instance on success.
 * \param alloc         The allocator to use for this operation.
 * \param name          The name of this transformer.
 * \param xform_fn      The transformer function to use for this instance.
 *
 * \note This transformer instance is a \ref resource that must be released by
 * calling \ref resource_release on its resource handle when it is no longer
 * needed by the caller. The resource handle can be accessed by calling \ref
 * transformer_resource_handle on this transformer instance.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 *
 * \pre
 *      - \p xform must not reference a valid \ref transformer instance and
 *        must not be NULL.
 *      - \p alloc must reference a valid \ref allocator and must not be NULL.
 *      - \p name must be a valid string and must not be NULL.
 *      - \p xform_fn must be a valid function and must not be NULL.
 * \post
 *      - On success, \p xform is set to a pointer to a valid
 *        \ref transformer instance, which is a \ref resource owned by the
 *        caller that must be released when no longer needed.
 *      - On failure, \p xform is set to NULL and an error status is returned.
 */
status FN_DECL_MUST_CHECK
transformer_create(
    transformer** xform, RCPR_SYM(allocator)* alloc, const char* name,
    transformer_fn xform_fn);

/**
 * \brief Get the singleton transformer registry implementation.
 *
 * \param reg           Pointer to the \ref transformer_registry pointer to be
 *                      set with the singleton instance on success.
 *
 * \note This method optionally allocates a registry instance. This instance
 * exists as a global. This allocation occurs only once, on a single thread. All
 * other threads will receive this single copy.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_singleton_get(
    transformer_registry** reg);

/******************************************************************************/
/* Start of accessors.                                                        */
/******************************************************************************/

/**
 * \brief Given a \ref transformer instance, return the resource handle for this
 * \ref transformer instance.
 *
 * \param xform         The \ref transformer instance from which the resource
 *                      handle is returned.
 *
 * \returns the resource handle for this \ref transformer instance.
 */
RCPR_SYM(resource)*
transformer_resource_handle(
    transformer* xform);

/**
 * \brief Register a transformer with the registry.
 *
 * \note The registry takes ownership of this transformer instance on success.
 *
 * \param xform         The \ref transformer instance to register.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_register(
    transformer* xform);

/**
 * \brief Look up a transformer by name from the registry.
 *
 * \note This transformer is owned by the registry.
 *
 * \param xform         Pointer to the \ref transformer pointer to be set with
 *                      the transformer if found.
 * \param name          The name of the transformer to look up.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
status FN_DECL_MUST_CHECK
transformer_registry_lookup(
    transformer** xform, const char* name);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
