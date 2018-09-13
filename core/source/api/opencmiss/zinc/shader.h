/**
 * @file shader.h
 *
 * Public interface to shader objects. Shaders provides additional functions to
 * change and add special effects to graphics but it may also provide non-graphics
 * related functions.
 *
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMZN_SHADER_H__
#define CMZN_SHADER_H__

#include "types/shaderid.h"
#include "opencmiss/zinc/zincsharedobject.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Returns a new handle to the shader module with reference count
 * incremented.
 *
 * @param shadermodule  Handle to shader module.
 * @return  New handle to shader module, or NULL/invalid handle on failure.
 */
ZINC_API cmzn_shadermodule_id cmzn_shadermodule_access(
    cmzn_shadermodule_id shadermodule);

/**
 * Destroys this handle to the shader module (and sets it to NULL).
 * Internally this decrements the reference count.
 *
 * @param shadermodule_address  Address of handle to shader module
 *   to destroy.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shadermodule_destroy(
    cmzn_shadermodule_id *shadermodule_address);

/**
 * Create and return a new shader uniforms.
 *
 * @param shadermodule  The handle to the shader module the
 * shader will belong to.
 * @return  Handle to new shader program, or NULL/invalid handle on failure.
 */
ZINC_API cmzn_shaderuniforms_id cmzn_shadermodule_create_shaderuniforms(
    cmzn_shadermodule_id shadermodule);

/**
 * Begin caching or increment cache level for this shader module. Call this
 * function before making multiple changes to minimise number of change messages
 * sent to clients. Must remember to end_change after completing changes.
 * Can be nested.
 * @see cmzn_shadermodule_end_change
 *
 * @param shadermodule  The shader module to begin change cache on.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shadermodule_begin_change(cmzn_shadermodule_id shadermodule);

/**
 * Decrement cache level or end caching of changes for the shader module.
 * Call shader module begin change method before making multiple changes
 * and call this afterwards. When change level is restored to zero,
 * cached change messages are sent out to clients.
 * @see cmzn_shadermodule_begin_change
 *
 * @param shadermodule  The shader module to end change cache on.
 * @return  Status CMZN_OK on success, any other value on failure.
 */
ZINC_API int cmzn_shadermodule_end_change(cmzn_shadermodule_id shadermodule);

/**
 * Find the shader uniforms with the specified name, if any.
 *
 * @param shadermodule  Shader module to search.
 * @param name  The name of the shader uniforms.
 * @return  Handle to shader, or NULL/invalid handle if not found or failed.
 */
ZINC_API cmzn_shaderuniforms_id cmzn_shadermodule_find_shaderuniforms_by_name(
    cmzn_shadermodule_id shadermodule, const char *name);

/**
 * Returns a new handle to the shaderuniforms with reference count incremented.
 *
 * @param shaderuniforms  Handle to shaderuniforms.
 * @return  New handle to shaderuniforms, or NULL/invalid handle on failure.
 */
ZINC_API cmzn_shaderuniforms_id cmzn_shaderuniforms_access(cmzn_shaderuniforms_id shaderuniforms);

/**
 * Destroys handle to the shaderuniforms (and sets it to NULL).
 * Internally this decrements the reference count.
 *
 * @param shaderuniforms_address  The address to the handle of the shaderuniforms
 *    to be destroyed.
 * @return  Status CMZN_OK on success, any other value on failure.
 */
ZINC_API int cmzn_shaderuniforms_destroy(cmzn_shaderuniforms_id *shaderuniforms_address);

/**
 * Get managed status of shaderuniforms in its owning shader module.
 * @see cmzn_shaderuniforms_set_managed
 *
 * @param shaderuniforms  The shaderuniforms to query.
 * @return  1 (true) if shaderuniforms is managed, otherwise 0 (false).
 */
ZINC_API bool cmzn_shaderuniforms_is_managed(cmzn_shaderuniforms_id shaderuniforms);

/**
 * Set managed status of shaderuniforms in its owning shader module.
 * If set (managed) the shaderuniforms will remain indefinitely in the shaderuniforms
 * module even if no external references are held.
 * If not set (unmanaged) the shaderuniforms will be automatically removed from the
 * module when no longer referenced externally, effectively marking it as
 * pending destruction.
 * All new objects are unmanaged unless stated otherwise.
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param value  The new value for the managed flag: true or false.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_set_managed(cmzn_shaderuniforms_id shaderuniforms,
    bool value);

/**
 * Remove a set of uniform from the uniforms object,
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param name  uniform with the matching name will be removed
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_remove_uniform(cmzn_shaderuniforms_id shaderuniforms,
	const char *name);

/**
 * Return the size and values of the specified uniform if present.
 *
 * @param shaderuniforms  The shaderuniforms to query.
 * @param valuesCount  The size of the valuesOut array to fill, from 1 to 4.
 * @param valuesOut  Array to receive uniform values.
 * @return  The size of unifomr , or 0 on error.
 */
ZINC_API int cmzn_shaderuniforms_get_uniform_double(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, double *valuesOut);

/**
 * Add a new set of double precision uniform to the uniforms object,
 * this set of values will be written into the shaders at runtime
 * when it is used by the shader program.
 * If an uniform with the same name already exist, nothing will be added to the
 * uniforms object.
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param name  name of the uniform to add
 * @param valuesCount  The size of the valuesIn array to fill, from 1 to 4.
 * @param values  Initial values to be set for the uniform.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_add_uniform_double(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, double *valuesIn);

/**
 * Set values of the specified uniform if it is found with
 * the specified name. This will not change the size of the uniform.
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param name  name of the uniform to set.
 * @param valuesCount  The size of the valuesIn array to fill, from 1 to 4.
 * @param values  Array containing values to be set for the uniform.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_set_uniform_double(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, double *valuesIn);

/**
 * Return the size and values of the specified uniform if present.
 *
 * @param shaderuniforms  The shaderuniforms to query.
 * @param valuesCount  The size of the valuesOut array to fill, from 1 to 4.
 * @param valuesOut  Array to receive uniform values.
 * @return  The size of unifomr , or 0 on error.
 */
ZINC_API int cmzn_shaderuniforms_get_uniform_integer(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, int *valuesOut);

/**
 * Add a new set of double precision uniform to the uniforms object,
 * this set of values will be written into the shaders at runtime
 * when it is used by the shader program.
 * If an uniform with the same name already exist, nothing will be added to the
 * uniforms object.
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param name  name of the uniform to add
 * @param valuesCount  The size of the valuesIn array to fill, from 1 to 4.
 * @param values  Initial values to be set for the uniform.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_add_uniform_integer(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, int *valuesIn);

/**
 * Set values of the specified uniform if it is found with
 * the specified name. This will not change the size of the uniform.
 *
 * @param shaderuniforms  The shaderuniforms to modify.
 * @param name  name of the uniform to set.
 * @param valuesCount  The size of the valuesIn array to fill, from 1 to 4.
 * @param values  Array containing values to be set for the uniform.
 * @return  Status CMZN_OK on success, otherwise CMZN_ERROR_ARGUMENT.
 */
ZINC_API int cmzn_shaderuniforms_set_uniform_integer(cmzn_shaderuniforms_id shaderuniforms,
	const char *name, int valuesCount, int *valuesIn);

/**
 * Return an allocated string containing shaderuniforms name.
 *
 * @param shaderuniforms  handle to the zinc shaderuniforms.
 * @return  allocated string containing shaderuniforms name, or NULL on failure.
 * Up to caller to free using cmzn_deallocate().
 */
ZINC_API char *cmzn_shaderuniforms_get_name(cmzn_shaderuniforms_id shaderuniforms);

/**
 * Set/change name for shaderuniforms. Must be unique in the shaderuniforms module.
 *
 * @param shaderuniforms  The handle to zinc shaderuniforms.
 * @param name  name to be set to the shaderuniforms
 * @return  status CMZN_OK if successfully set/change name for shaderuniforms,
 * any other value on failure.
 */
ZINC_API int cmzn_shaderuniforms_set_name(cmzn_shaderuniforms_id shaderuniforms, const char *name);

#ifdef __cplusplus
}
#endif

#endif
