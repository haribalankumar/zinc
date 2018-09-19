/***************************************************************************//**
 * shader_uniforms.hpp
 *
 * Objects for providing shaders uniforms values.
 *
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if !defined (SHADER_UNIFORMS_HPP)
#define SHADER_UNIFORMS_HPP

#include "opencmiss/zinc/shader.h"
#include "general/list.h"
#include "general/manager.h"
#include "general/object.h"

struct cmzn_shaderuniforms;

struct cmzn_shaderuniforms *cmzn_shaderuniforms_create_private();

void cmzn_shaderuniforms_write_to_shaders(cmzn_shaderuniforms_id shaderuniforms,
	unsigned int glsl_program);

/***************************************************************************//**
 * Private; only to be called from graphics_module.
 */
int cmzn_shaderuniforms_manager_set_owner_private(struct MANAGER(cmzn_shaderuniforms) *manager,
	struct cmzn_shadernmodule *shadermodule);

DECLARE_LIST_TYPES(cmzn_shaderuniforms);
DECLARE_MANAGER_TYPES(cmzn_shaderuniforms);

PROTOTYPE_OBJECT_FUNCTIONS(cmzn_shaderuniforms);
PROTOTYPE_GET_OBJECT_NAME_FUNCTION(cmzn_shaderuniforms);

PROTOTYPE_LIST_FUNCTIONS(cmzn_shaderuniforms);
PROTOTYPE_FIND_BY_IDENTIFIER_IN_LIST_FUNCTION(cmzn_shaderuniforms,name,const char *);

PROTOTYPE_MANAGER_FUNCTIONS(cmzn_shaderuniforms);
PROTOTYPE_MANAGER_IDENTIFIER_WITHOUT_MODIFY_FUNCTIONS(cmzn_shaderuniforms,name,const char *);

#endif
