/***************************************************************************//**
 * FILE : material.hpp
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef CMZN_MATERIAL_HPP__
#define CMZN_MATERIAL_HPP__

#include "zinc/material.h"
#include "zinc/fieldimage.h"
#include "zinc/field.hpp"

namespace OpenCMISS
{
namespace Zinc
{

class Material
{

protected:
	cmzn_material_id id;

public:

	Material() : id(0)
	{ }

	// takes ownership of C handle, responsibility for destroying it
	explicit Material(cmzn_material_id material_id) :
		id(material_id)
	{ }

	Material(const Material& material) :
		id(cmzn_material_access(material.id))
	{ }

	Material& operator=(const Material& material)
	{
		cmzn_material_id temp_id = cmzn_material_access(material.id);
		if (0 != id)
		{
			cmzn_material_destroy(&id);
		}
		id = temp_id;
		return *this;
	}

	~Material()
	{
		if (0 != id)
		{
			cmzn_material_destroy(&id);
		}
	}

	bool isValid()
	{
		return (0 != id);
	}

	cmzn_material_id getId()
	{
		return id;
	}

	enum Attribute
	{
		ATTRIBUTE_INVALID = CMZN_MATERIAL_ATTRIBUTE_INVALID,
		ATTRIBUTE_ALPHA = CMZN_MATERIAL_ATTRIBUTE_ALPHA,
		ATTRIBUTE_AMBIENT = CMZN_MATERIAL_ATTRIBUTE_AMBIENT,
		ATTRIBUTE_DIFFUSE = CMZN_MATERIAL_ATTRIBUTE_DIFFUSE,
		ATTRIBUTE_EMISSION = CMZN_MATERIAL_ATTRIBUTE_EMISSION,
		ATTRIBUTE_SHININESS = CMZN_MATERIAL_ATTRIBUTE_SHININESS,
		ATTRIBUTE_SPECULAR = CMZN_MATERIAL_ATTRIBUTE_SPECULAR
	};

	bool isManaged()
	{
		return cmzn_material_is_managed(id);
	}

	int setManaged(bool value)
	{
		return cmzn_material_set_managed(id, value);
	}

	double getAttributeReal(Attribute attribute)
	{
		return cmzn_material_get_attribute_real(id,
			static_cast<cmzn_material_attribute>(attribute));
	}

	int setAttributeReal(Attribute attribute, double value)
	{
		return cmzn_material_set_attribute_real(id,
			static_cast<cmzn_material_attribute>(attribute), value);
	}

	int getAttributeReal3(Attribute attribute, double *valuesOut3)
	{
		return cmzn_material_get_attribute_real3(id,
			static_cast<cmzn_material_attribute>(attribute), valuesOut3);
	}

	int setAttributeReal3(Attribute attribute, const double *valuesIn3)
	{
		return cmzn_material_set_attribute_real3(id,
			static_cast<cmzn_material_attribute>(attribute), valuesIn3);
	}

	char *getName()
	{
		return cmzn_material_get_name(id);
	}

	int setName(const char *name)
	{
		return cmzn_material_set_name(id, name);
	}

	int setImageField(int imageNumber, Field &imageField)
	{
		cmzn_field_image_id field_image = cmzn_field_cast_image(imageField.getId());
		int result = cmzn_material_set_image_field(id, imageNumber, field_image);
		cmzn_field_image_destroy(&field_image);

		return result;
	}

};

class Materialmodule
{
protected:
	cmzn_materialmodule_id id;

public:

	Materialmodule() : id(0)
	{  }

	// takes ownership of C handle, responsibility for destroying it
	explicit Materialmodule(cmzn_materialmodule_id in_materialmodule_id) :
		id(in_materialmodule_id)
	{  }

	Materialmodule(const Materialmodule& materialModule) :
		id(cmzn_materialmodule_access(materialModule.id))
	{  }

	Materialmodule& operator=(const Materialmodule& materialModule)
	{
		cmzn_materialmodule_id temp_id = cmzn_materialmodule_access(
			materialModule.id);
		if (0 != id)
		{
			cmzn_materialmodule_destroy(&id);
		}
		id = temp_id;
		return *this;
	}

	~Materialmodule()
	{
		if (0 != id)
		{
			cmzn_materialmodule_destroy(&id);
		}
	}

	bool isValid()
	{
		return (0 != id);
	}

	cmzn_materialmodule_id getId()
	{
		return id;
	}

	Material createMaterial()
	{
		return Material(cmzn_materialmodule_create_material(id));
	}

	Material findMaterialByName(const char *name)
	{
		return Material(cmzn_materialmodule_find_material_by_name(id, name));
	}

	int beginChange()
	{
		return cmzn_materialmodule_begin_change(id);
	}

	int endChange()
	{
		return cmzn_materialmodule_end_change(id);
	}

	int defineStandardMaterials()
	{
		return cmzn_materialmodule_define_standard_materials(id);
	}

	Material getDefaultMaterial()
	{
		return Material(cmzn_materialmodule_get_default_material(id));
	}

	int setDefaultMaterial(Material &material)
	{
		return cmzn_materialmodule_set_default_material(id, material.getId());
	}

	Material getDefaultSelectedMaterial()
	{
		return Material(cmzn_materialmodule_get_default_selected_material(id));
	}

	int setDefaultSelectedMaterial(Material &material)
	{
		return cmzn_materialmodule_set_default_selected_material(id, material.getId());
	}
};

} // namespace cmzn
}
#endif
