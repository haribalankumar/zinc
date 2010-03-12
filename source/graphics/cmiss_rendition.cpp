/*******************************************************************************
FILE : cmiss_rendition.cpp

==============================================================================*/
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is cmgui.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

extern "C" {
#include "api/cmiss_graphics_module.h"
#include "finite_element/finite_element.h"
#include "general/debug.h"
#include "general/object.h"
#include "graphics/cmiss_rendition.h"
#include "graphics/glyph.h"
#include "graphics/graphics_object.h"
#include "graphics/element_point_ranges.h"
#include "graphics/font.h"
#include "graphics/material.h"
#include "graphics/scene.h"
#include "graphics/spectrum.h"
#include "time/time_keeper.h"
#include "user_interface/message.h"
}

struct Cmiss_graphics_module
{
	/* attribute managers and defaults: */
	struct LIST(GT_object) *glyph_list;
	struct LIST(GT_object) *graphics_object_list;
	struct Material_package *material_package;
	struct Graphics_font *default_font;
	struct Graphics_font_package *graphics_font_package;
	struct Light *default_light;
	struct MANAGER(Light) *light_manager;
	struct LIST(Light) *list_of_lights;
	struct MANAGER(Spectrum) *spectrum_manager;
	struct Spectrum *default_spectrum;
	struct MANAGER(Texture) *texture_manager; 
	struct Time_keeper *default_time_keeper;
	struct MANAGER(Scene) *scene_manager;
	struct Scene *default_scene;
	struct Light_model *default_light_model;
	struct MANAGER(Light_model) *light_model_manager;
	int access_count;
};

struct Cmiss_graphics_module *Cmiss_graphics_module_create(
	struct Context *context)
{
	struct Cmiss_graphics_module *module;

	ENTER(Cmiss_graphics_module_create);
	if (context)
	{
		if (ALLOCATE(module, struct Cmiss_graphics_module, 1))
		{
			module->texture_manager = NULL;
			module->light_manager = NULL;
			module->spectrum_manager = NULL;
			module->material_package = NULL;
			module->list_of_lights = NULL;
			module->glyph_list = NULL;
			module->default_font = NULL;
			module->default_light = NULL;
			module->default_spectrum = NULL;
			module->default_time_keeper = NULL;
			module->graphics_object_list=NULL;
			module->graphics_font_package = NULL;
			module->default_scene = NULL;
			module->default_light_model = NULL;
			module->texture_manager=CREATE(MANAGER(Texture))();
			module->light_manager=CREATE(MANAGER(Light))();
			module->spectrum_manager=CREATE(MANAGER(Spectrum))();
			module->material_package = ACCESS(Material_package)(CREATE(Material_package)
				(module->texture_manager, module->spectrum_manager));
			module->scene_manager = CREATE(MANAGER(Scene)());
			module->light_model_manager = CREATE(MANAGER(Light_model)());
			module->access_count = 1;
		}
		else
		{
			module = (Cmiss_graphics_module *)NULL;
			display_message(ERROR_MESSAGE,
			"Cmiss_rendtion_graphics_module_create. Not enough memory for Cmiss rendition graphics module");
		}
	}
	else
	{
		module = (Cmiss_graphics_module *)NULL;
		display_message(ERROR_MESSAGE,"Cmiss_rendtion_graphics_module_create.  Invalid argument(s)");
	}
	LEAVE;

	return (module);
}

struct Material_package *Cmiss_graphics_module_get_material_package(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Material_package *material_package = NULL;
	if (graphics_module && graphics_module->material_package)
	{
		material_package = ACCESS(Material_package)(graphics_module->material_package);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_material_package.  Invalid argument(s)");
	}

	return material_package;
}

struct MANAGER(Graphical_material) *Cmiss_graphics_module_get_material_manager(
	struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Graphical_material) *material_manager;

	ENTER(Cmiss_graphics_module_get_material_manager);
	if (graphics_module && graphics_module->material_package)
	{
		material_manager = 	Material_package_get_material_manager(
			graphics_module->material_package);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_material_manager.  Invalid argument(s)");
		material_manager = (struct MANAGER(Graphical_material) *)NULL;
	}
	LEAVE;

	return (material_manager);
}

struct Cmiss_graphics_module *Cmiss_graphics_module_access(
	struct Cmiss_graphics_module *graphics_module)
{
	if (graphics_module)
	{
		graphics_module->access_count++;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_access.  Invalid argument(s)");
	}
	return graphics_module;
}

int Cmiss_graphics_module_destroy(
	struct Cmiss_graphics_module **graphics_module_address)
{
	int return_code = 0;
	struct Cmiss_graphics_module *graphics_module = NULL;

	if (NULL != (graphics_module = *graphics_module_address))
	{
		graphics_module->access_count--;
		if (0 == graphics_module->access_count)
		{
			if (graphics_module->default_scene)
				DEACCESS(Scene)(&graphics_module->default_scene);
 			if (graphics_module->scene_manager)
 				DESTROY(MANAGER(Scene))(&graphics_module->scene_manager);
			if (graphics_module->graphics_object_list)
				DESTROY(LIST(GT_object))(&graphics_module->graphics_object_list);
 			if (graphics_module->glyph_list)
 				DESTROY(LIST(GT_object))(&graphics_module->glyph_list);
			if (graphics_module->default_light)
				DEACCESS(Light)(&graphics_module->default_light);
			if (graphics_module->light_manager)
				DESTROY(MANAGER(Light))(&graphics_module->light_manager);
			if (graphics_module->default_light_model)
				DEACCESS(Light_model)(&graphics_module->default_light_model);
			if (graphics_module->light_model_manager)
				DESTROY(MANAGER(Light_model))(&graphics_module->light_model_manager);
			if (graphics_module->default_spectrum)
				DEACCESS(Spectrum)(&graphics_module->default_spectrum);
			if (graphics_module->spectrum_manager)
				DESTROY(MANAGER(Spectrum))(&graphics_module->spectrum_manager);
			if (graphics_module->material_package)
				DEACCESS(Material_package)(&graphics_module->material_package);
			if (graphics_module->default_font)
				DEACCESS(Graphics_font)(&graphics_module->default_font);
			if (graphics_module->texture_manager)
				DESTROY(MANAGER(Texture))(&graphics_module->texture_manager);
			if (graphics_module->graphics_font_package)
				DESTROY(Graphics_font_package)(&graphics_module->graphics_font_package);
			if (graphics_module->default_time_keeper)
				DEACCESS(Time_keeper)(&graphics_module->default_time_keeper);
			DEALLOCATE(*graphics_module_address);
		}
		*graphics_module_address = NULL;
		return_code = 1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_destroy.  Missing graphics module");
		return_code = 0;
	}

	return return_code;
}

struct MANAGER(Texture) *Cmiss_graphics_module_get_texture_manager(
	struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Texture) *texture_manager = NULL;
	if (graphics_module)
	{
		texture_manager = graphics_module->texture_manager;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_texture_manager.  Invalid argument(s)");
	}
	return texture_manager;
}

struct MANAGER(Light) *Cmiss_graphics_module_get_light_manager(
	struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Light) *light_manager = NULL;
	if (graphics_module)
	{
		light_manager = graphics_module->light_manager;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_light_manager.  Invalid argument(s)");
	}
	return light_manager;
}

struct Light *Cmiss_graphics_module_get_default_light(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Light *light = NULL;
	if (graphics_module && graphics_module->light_manager)
	{
		if (!graphics_module->default_light)
		{
			graphics_module->default_light=CREATE(Light)("default");
			if (graphics_module->default_light)
			{
				float default_light_direction[3]={0.0,-0.5,-1.0};
				struct Colour default_colour;
				set_Light_type(graphics_module->default_light,INFINITE_LIGHT);
				default_colour.red=1.0;
				default_colour.green=1.0;
				default_colour.blue=1.0;
				set_Light_colour(graphics_module->default_light,&default_colour); 
				
				set_Light_direction(graphics_module->default_light,default_light_direction);
				/*???DB.  Include default as part of manager ? */
				ACCESS(Light)(graphics_module->default_light);
				if (!ADD_OBJECT_TO_MANAGER(Light)(graphics_module->default_light,
						graphics_module->light_manager))
				{
					DEACCESS(Light)(&(graphics_module->default_light));
				}
			}
		}
		if (graphics_module->default_light)
		{
			light = ACCESS(Light)(graphics_module->default_light);
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_light.  Invalid argument(s)");
	}

	return light;
}

struct MANAGER(Spectrum) *Cmiss_graphics_module_get_spectrum_manager(
	struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Spectrum) *spectrum_manager = NULL;
	if (graphics_module)
	{
		spectrum_manager = graphics_module->spectrum_manager;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_spectrum_manager.  Invalid argument(s)");
	}

	return spectrum_manager;
}

struct Spectrum *Cmiss_graphics_module_get_default_spectrum(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Spectrum *spectrum = NULL;

	if (graphics_module && graphics_module->spectrum_manager)
	{
		if (!graphics_module->default_spectrum)
		{
			graphics_module->default_spectrum=CREATE(Spectrum)("default");
			if (graphics_module->default_spectrum)
			{
				Spectrum_set_simple_type(graphics_module->default_spectrum,
					BLUE_TO_RED_SPECTRUM);
				Spectrum_set_minimum_and_maximum(graphics_module->default_spectrum,0,1);
				/* ACCESS so can never be destroyed */
				ACCESS(Spectrum)(graphics_module->default_spectrum);
				if (!ADD_OBJECT_TO_MANAGER(Spectrum)(graphics_module->default_spectrum,
						graphics_module->spectrum_manager))
				{
					DEACCESS(Spectrum)(&(graphics_module->default_spectrum));
				}
			}
		}
		if (graphics_module->default_spectrum)
		{
			spectrum = ACCESS(Spectrum)(graphics_module->default_spectrum);
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_spectrum.  Invalid argument(s)");
	}

	return spectrum;
}

struct Startup_material_definition
{
	const char *name;
	MATERIAL_PRECISION ambient[3];
	MATERIAL_PRECISION diffuse[3];
	MATERIAL_PRECISION emission[3];
	MATERIAL_PRECISION specular[3];
	MATERIAL_PRECISION alpha;
	MATERIAL_PRECISION shininess;
};

int Cmiss_graphics_module_create_standard_materials(
	struct Cmiss_graphics_module *graphics_module)
{
	/* only the default material is not in this list because its colour changes
		 to contrast with the background; colours are R G B */
	
	struct Startup_material_definition
		startup_materials[] =
		{
			{"black",
			 /*ambient*/ { 0.00, 0.00, 0.00},
			 /*diffuse*/ { 0.00, 0.00, 0.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.30, 0.30, 0.30},
	 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"blue",
			 /*ambient*/ { 0.00, 0.00, 0.50},
			 /*diffuse*/ { 0.00, 0.00, 1.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.20, 0.20, 0.20},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"bone",
			 /*ambient*/ { 0.70, 0.70, 0.60},
			 /*diffuse*/ { 0.90, 0.90, 0.70},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.10, 0.10, 0.10},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"gray50",
			 /*ambient*/ { 0.50, 0.50, 0.50},
			 /*diffuse*/ { 0.50, 0.50, 0.50},
			 /*emission*/{ 0.50, 0.50, 0.50},
			 /*specular*/{ 0.50, 0.50, 0.50},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"gold",
			 /*ambient*/ { 1.00, 0.40, 0.00},
			 /*diffuse*/ { 1.00, 0.70, 0.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.50, 0.50, 0.50},
			 /*alpha*/1.0,
			 /*shininess*/0.3},
			{"green",
			 /*ambient*/ { 0.00, 0.50, 0.00},
			 /*diffuse*/ { 0.00, 1.00, 0.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.20, 0.20, 0.20},
			 /*alpha*/1.0,
			 /*shininess*/0.1},
			{"muscle",
			 /*ambient*/ { 0.40, 0.14, 0.11},
			 /*diffuse*/ { 0.50, 0.12, 0.10},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.30, 0.50, 0.50},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"red",
			 /*ambient*/ { 0.50, 0.00, 0.00},
			 /*diffuse*/ { 1.00, 0.00, 0.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.20, 0.20, 0.20},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			{"silver",
			 /*ambient*/ { 0.40, 0.40, 0.40},
			 /*diffuse*/ { 0.70, 0.70, 0.70},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.50, 0.50, 0.50},
			 /*alpha*/1.0,
			 /*shininess*/0.3},
			{"tissue",
			 /*ambient*/ { 0.90, 0.70, 0.50},
			 /*diffuse*/ { 0.90, 0.70, 0.50},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.20, 0.20, 0.30},
			 /*alpha*/1.0,
			 /*shininess*/0.2},
			/* Used as the default fail_material for texture evaluation. */
			{"transparent_gray50",
			 /*ambient*/ { 0.50, 0.50, 0.50},
			 /*diffuse*/ { 0.50, 0.50, 0.50},
			 /*emission*/{ 0.50, 0.50, 0.50},
			 /*specular*/{ 0.50, 0.50, 0.50},
			 /*alpha*/0.0,
			 /*shininess*/0.2},
			{"white",
			 /*ambient*/ { 1.00, 1.00, 1.00},
			 /*diffuse*/ { 1.00, 1.00, 1.00},
			 /*emission*/{ 0.00, 0.00, 0.00},
			 /*specular*/{ 0.00, 0.00, 0.00},
			 /*alpha*/1.0,
			 /*shininess*/0.0}
		};
	int i, return_code;
	int number_of_startup_materials = sizeof(startup_materials) /
		sizeof(struct Startup_material_definition);
	struct Graphical_material *material;
	struct Colour colour;
	
	if (graphics_module && graphics_module->material_package)
	{
		for (i = 0; i < number_of_startup_materials; i++)
		{
			if (NULL != (material = CREATE(Graphical_material)(startup_materials[i].name)))
			{
					colour.red   = startup_materials[i].ambient[0];
					colour.green = startup_materials[i].ambient[1];
					colour.blue  = startup_materials[i].ambient[2];
					Graphical_material_set_ambient(material, &colour);
					colour.red   = startup_materials[i].diffuse[0];
					colour.green = startup_materials[i].diffuse[1];
					colour.blue  = startup_materials[i].diffuse[2];
					Graphical_material_set_diffuse(material, &colour);
					colour.red   = startup_materials[i].emission[0];
					colour.green = startup_materials[i].emission[1];
					colour.blue  = startup_materials[i].emission[2];
					Graphical_material_set_emission(material, &colour);
					colour.red   = startup_materials[i].specular[0];
					colour.green = startup_materials[i].specular[1];
					colour.blue  = startup_materials[i].specular[2];
					Graphical_material_set_specular(material, &colour);
					Graphical_material_set_alpha(material, startup_materials[i].alpha);
					Graphical_material_set_shininess(material,
						startup_materials[i].shininess);
					if (!Material_package_manage_material(graphics_module->material_package,
							material))
					{
					DESTROY(Graphical_material)(&material);
				}
			}
		}

		return_code = 1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_create_standard_material.  Invalid argument(s)");
		return_code = 0;
	}

	
	return return_code;
}

struct LIST(GT_object) * Cmiss_graphics_module_get_default_GT_object_list(
	struct Cmiss_graphics_module *graphics_module)
{
	LIST(GT_object) *graphics_object_list = NULL;
	if (graphics_module)
	{
		if (!graphics_module->graphics_object_list)
		{
			graphics_module->graphics_object_list=CREATE(LIST(GT_object))();
		}
		graphics_object_list = graphics_module->graphics_object_list;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_GT_object_list.  Invalid argument(s)");
	}
	return (graphics_object_list);
}

struct Graphics_font_package *Cmiss_graphics_module_get_font_package(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Graphics_font_package *graphics_font_package = NULL;
	if (graphics_module)
	{
		if (!graphics_module->graphics_font_package)
		{
			graphics_module->graphics_font_package=CREATE(Graphics_font_package)();
		}
		graphics_font_package = graphics_module->graphics_font_package;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_font_package.  Invalid argument(s)");
	}
	
	return (graphics_font_package);
}

struct Graphics_font *Cmiss_graphics_module_get_default_font(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Graphics_font *default_font = NULL;
	if (graphics_module)
	{
		if (!graphics_module->default_font)
		{
			graphics_module->default_font=ACCESS(Graphics_font)(
				Graphics_font_package_get_font(graphics_module->graphics_font_package, "default"));
		}
		default_font = ACCESS(Graphics_font)(graphics_module->default_font);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_font.  Invalid argument(s)");
	}

	return (default_font);
}

struct LIST(GT_object) * Cmiss_graphics_module_get_default_glyph_list(
		struct Cmiss_graphics_module *graphics_module)
{
	LIST(GT_object) *glyph_list = NULL;
	if (graphics_module)
	{
		if (!graphics_module->glyph_list)
		{
			struct Graphics_font *default_font = Cmiss_graphics_module_get_default_font(
				graphics_module);
			graphics_module->glyph_list=make_standard_glyphs(default_font);
			DEACCESS(Graphics_font)(&default_font);
		}
		glyph_list = graphics_module->glyph_list;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_glyph_list.  Invalid argument(s)");
	}
	return (glyph_list);
}

struct MANAGER(Scene) *Cmiss_graphics_module_get_scene_manager(
		struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Scene) *scene_manager = NULL;

	if (graphics_module)
	{
		if (!graphics_module->scene_manager)
		{
			graphics_module->scene_manager=CREATE(MANAGER(Scene)());
		}
		scene_manager = graphics_module->scene_manager;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_scene_manager.  Invalid argument(s)");
	}

	return scene_manager;
}

struct Scene *Cmiss_graphics_module_get_default_scene(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Scene *scene = NULL;
	if (graphics_module)
	{
		if (!graphics_module->default_scene)
		{
			if (NULL != (graphics_module->default_scene=ACCESS(Scene)(CREATE(Scene)("default"))))
			{
				struct MANAGER(Scene) *scene_manager = 
					Cmiss_graphics_module_get_scene_manager(graphics_module);
				if (!ADD_OBJECT_TO_MANAGER(Scene)(graphics_module->default_scene,
						scene_manager))
				{
					DEACCESS(Scene)(&(graphics_module->default_scene));
				}
			}
		}
		if (graphics_module->default_scene)
			scene = ACCESS(Scene)(graphics_module->default_scene);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_scene.  Invalid argument(s)");
	}

	return scene;
}

struct MANAGER(Light_model) *Cmiss_graphics_module_get_light_model_manager(
	struct Cmiss_graphics_module *graphics_module)
{
	struct MANAGER(Light_model) *light_model_manager = NULL;
	if (graphics_module)
	{
		light_model_manager = graphics_module->light_model_manager;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_light_model_manager.  Invalid argument(s)");
	}

	return light_model_manager;
}

struct Light_model *Cmiss_graphics_module_get_default_light_model(
	struct Cmiss_graphics_module *graphics_module)
{
	struct Light_model *light_model = NULL;

	if (graphics_module)
	{
		if (!graphics_module->default_light_model)
		{
			if (NULL != (graphics_module->default_light_model=CREATE(Light_model)("default")))
			{
				struct Colour ambient_colour;
				ambient_colour.red=0.2;
				ambient_colour.green=0.2;
				ambient_colour.blue=0.2;
				Light_model_set_ambient(graphics_module->default_light_model,&ambient_colour);
				Light_model_set_side_mode(graphics_module->default_light_model,
					LIGHT_MODEL_TWO_SIDED);		
				ACCESS(Light_model)(graphics_module->default_light_model);
				if (!ADD_OBJECT_TO_MANAGER(Light_model)(
							graphics_module->default_light_model,graphics_module->light_model_manager))
				{
					DEACCESS(Light_model)(&(graphics_module->default_light_model));
				}
			}
		}
		if (graphics_module->default_light_model)
			light_model = ACCESS(Light_model)(graphics_module->default_light_model);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Cmiss_graphics_module_get_default_light_model.  Invalid argument(s)");
	}
	return light_model;
}
