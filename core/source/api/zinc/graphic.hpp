/***************************************************************************//**
 * FILE : graphic.hpp
 */
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
 * The Original Code is libZinc.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2012
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
#ifndef __ZN_CMISS_GRAPHIC_HPP__
#define __ZN_CMISS_GRAPHIC_HPP__

#include "zinc/graphic.h"
#include "zinc/field.hpp"
#include "zinc/graphicsfont.hpp"
#include "zinc/graphicsmaterial.hpp"
#include "zinc/spectrum.hpp"
#include "zinc/tessellation.hpp"

namespace zinc
{

class GraphicLineAttributes;
class GraphicPointAttributes;

class Graphic
{

protected:
	Cmiss_graphic_id id;

public:

	Graphic() : id(0)
	{  }

	// takes ownership of C handle, responsibility for destroying it
	explicit Graphic(Cmiss_graphic_id graphic_id) : id(graphic_id)
	{  }

	Graphic(const Graphic& graphic) : id(Cmiss_graphic_access(graphic.id))
	{  }

	Graphic& operator=(const Graphic& graphic)
	{
		Cmiss_graphic_id temp_id = Cmiss_graphic_access(graphic.id);
		if (0 != id)
		{
			Cmiss_graphic_destroy(&id);
		}
		id = temp_id;
		return *this;
	}

	~Graphic()
	{
		if (0 != id)
		{
			Cmiss_graphic_destroy(&id);
		}
	}

	bool isValid()
	{
		return (0 != id);
	}

	enum RenderType
	{
		RENDER_TYPE_INVALID = CMISS_GRAPHICS_RENDER_TYPE_INVALID,
		RENDER_TYPE_SHADED = CMISS_GRAPHICS_RENDER_TYPE_SHADED,
		RENDER_TYPE_WIREFRAME = CMISS_GRAPHICS_RENDER_TYPE_WIREFRAME
	};

	enum CoordinateSystem
	{
		COORDINATE_SYSTEM_INVALID = CMISS_GRAPHICS_COORDINATE_SYSTEM_INVALID,
		COORDINATE_SYSTEM_LOCAL = CMISS_GRAPHICS_COORDINATE_SYSTEM_LOCAL,
		COORDINATE_SYSTEM_WORLD = CMISS_GRAPHICS_COORDINATE_SYSTEM_WORLD,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FILL = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FILL,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_CENTRE = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_CENTRE,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_LEFT = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_LEFT,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_RIGHT = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_RIGHT,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_BOTTOM = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_BOTTOM,
		COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_TOP = CMISS_GRAPHICS_COORDINATE_SYSTEM_NORMALISED_WINDOW_FIT_TOP,
		COORDINATE_SYSTEM_WINDOW_PIXEL_BOTTOM_LEFT = CMISS_GRAPHICS_COORDINATE_SYSTEM_WINDOW_PIXEL_BOTTOM_LEFT,
		COORDINATE_SYSTEM_WINDOW_PIXEL_TOP_LEFT = CMISS_GRAPHICS_COORDINATE_SYSTEM_WINDOW_PIXEL_TOP_LEFT
	};

	enum GlyphType
	{
		GLYPH_TYPE_INVALID = CMISS_GRAPHICS_GLYPH_TYPE_INVALID,
		GLYPH_TYPE_NONE = CMISS_GRAPHICS_GLYPH_NONE,
		GLYPH_TYPE_POINT = CMISS_GRAPHICS_GLYPH_POINT,
		GLYPH_TYPE_LINE = CMISS_GRAPHICS_GLYPH_LINE,
		GLYPH_TYPE_SPHERE = CMISS_GRAPHICS_GLYPH_SPHERE,
		GLYPH_TYPE_AXES = CMISS_GRAPHICS_GLYPH_AXES_SOLID
	};

	enum GraphicType
	{
		GRAPHIC_TYPE_INVALID = CMISS_GRAPHIC_TYPE_INVALID,
		GRAPHIC_NODE_POINTS = CMISS_GRAPHIC_NODE_POINTS,
		GRAPHIC_DATA_POINTS = CMISS_GRAPHIC_DATA_POINTS,
		GRAPHIC_LINES = CMISS_GRAPHIC_LINES,
		GRAPHIC_CYLINDERS = CMISS_GRAPHIC_CYLINDERS,
		GRAPHIC_SURFACES = CMISS_GRAPHIC_SURFACES,
		GRAPHIC_ISO_SURFACES = CMISS_GRAPHIC_ISO_SURFACES,
		GRAPHIC_ELEMENT_POINTS = CMISS_GRAPHIC_ELEMENT_POINTS,
		GRAPHIC_STREAMLINES = CMISS_GRAPHIC_STREAMLINES,
		GRAPHIC_POINT = CMISS_GRAPHIC_POINT
	};

	enum UseElementType
	{
		USE_ELEMENT_TYPE_INVALID = CMISS_GRAPHIC_USE_ELEMENT_TYPE_INVALID,
		USE_ELEMENT_HIGHEST_DIMENSION = CMISS_GRAPHIC_USE_ELEMENT_HIGHEST_DIMENSION,
		USE_ELEMENT_FACES = CMISS_GRAPHIC_USE_ELEMENT_FACES,
		USE_ELEMENT_LINES = CMISS_GRAPHIC_USE_ELEMENT_LINES
	};

	enum FaceType
	{
		FACE_INVALID = CMISS_GRAPHIC_FACE_INVALID,
		FACE_ALL = CMISS_GRAPHIC_FACE_ALL,
		FACE_XI1_0 = CMISS_GRAPHIC_FACE_XI1_0,
		FACE_XI1_1 = CMISS_GRAPHIC_FACE_XI1_1,
		FACE_XI2_0 = CMISS_GRAPHIC_FACE_XI2_0,
		FACE_XI2_1 = CMISS_GRAPHIC_FACE_XI2_1,
		FACE_XI3_0 = CMISS_GRAPHIC_FACE_XI3_0,
		FACE_XI3_1 = CMISS_GRAPHIC_FACE_XI3_1
	};

	Cmiss_graphic_id getId()
	{
		return id;
	}

	Field getCoordinateField()
	{
		return Field(Cmiss_graphic_get_coordinate_field(id));
	}

	int setCoordinateField(Field& coordinateField)
	{
		return Cmiss_graphic_set_coordinate_field(id, coordinateField.getId());
	}

	Field getDataField()
	{
		return Field(Cmiss_graphic_get_data_field(id));
	}

	int setDataField(Field& dataField)
	{
		return Cmiss_graphic_set_data_field(id, dataField.getId());
	}

	int setTextureCoordinateField(Field& textureCoordinateField)
	{
		return Cmiss_graphic_set_texture_coordinate_field(id, textureCoordinateField.getId());
	}

	int setMaterial(GraphicsMaterial& graphicsMaterial)
	{
		return Cmiss_graphic_set_material(id, graphicsMaterial.getId());
	}

	GraphicLineAttributes getLineAttributes();

	GraphicPointAttributes getPointAttributes();

	int setSelectedMaterial(GraphicsMaterial& graphicsMaterial)
	{
		return Cmiss_graphic_set_selected_material(id, graphicsMaterial.getId());
	}

	Spectrum getSpectrum()
	{
		return Spectrum(Cmiss_graphic_get_spectrum(id));
	}

	int setSpectrum(Spectrum& spectrum)
	{
		return Cmiss_graphic_set_spectrum(id, spectrum.getId());
	}

	Tessellation getTessellation()
	{
		return Tessellation(Cmiss_graphic_get_tessellation(id));
	}

	int setTessellation(Tessellation& tessellation)
	{
		return Cmiss_graphic_set_tessellation(id, tessellation.getId());
	}

	enum RenderType getRenderType()
	{
		return static_cast<RenderType>(Cmiss_graphic_get_render_type(id));
	}

	int setRenderType(RenderType renderType)
	{
		return Cmiss_graphic_set_render_type(id,
			static_cast<Cmiss_graphics_render_type>(renderType));
	}

	bool getVisibilityFlag()
	{
		return (0 != Cmiss_graphic_get_visibility_flag(id));
	}

	int setVisibilityFlag(bool visibilityFlag)
	{
		return Cmiss_graphic_set_visibility_flag(id, (int)visibilityFlag);
	}

	enum CoordinateSystem getCoordinateSystem()
	{
		return static_cast<CoordinateSystem>(Cmiss_graphic_get_coordinate_system(id));
	}

	int setCoordinateSystem(CoordinateSystem coordinateSystem)
	{
		return Cmiss_graphic_set_coordinate_system(id,
			static_cast<Cmiss_graphics_coordinate_system>(coordinateSystem));
	}

	int setUseElementType(UseElementType useElementType)
	{
		return Cmiss_graphic_set_use_element_type(id, static_cast<Cmiss_graphic_use_element_type>(useElementType));
	}

	char *getName()
	{
		return Cmiss_graphic_get_name(id);
	}

	int setName(const char *name)
	{
		return Cmiss_graphic_set_name(id, name);
	}

	int setFace(FaceType face)
	{
		return Cmiss_graphic_set_face(id, static_cast<Cmiss_graphic_face_type>(face));
	}

	FaceType getFace()
	{
		return static_cast<FaceType>(Cmiss_graphic_get_face(id));
	}

	int setExterior(int exterior)
	{
		return Cmiss_graphic_set_exterior(id, exterior);
	}

	int getExterior()
	{
		return Cmiss_graphic_get_exterior(id);
	}
};

class GraphicIsoSurface : public Graphic
{
private:
	explicit GraphicIsoSurface(Cmiss_graphic_id graphic_id) : Graphic(graphic_id) {}

public:
	GraphicIsoSurface() : Graphic(0) {}

	explicit GraphicIsoSurface(Cmiss_graphic_iso_surface_id graphic_iso_surface_id)
		: Graphic(reinterpret_cast<Cmiss_graphic_id>(graphic_iso_surface_id))
	{}

	GraphicIsoSurface(Graphic& graphic)
		: Graphic(reinterpret_cast<Cmiss_graphic_id>(Cmiss_graphic_cast_iso_surface(graphic.getId())))
	{}

	Field getIsoScalarField()
	{
		return Field(Cmiss_graphic_iso_surface_get_iso_scalar_field(reinterpret_cast<Cmiss_graphic_iso_surface_id>(id)));
	}

	int setIsoScalarField(Field& field)
	{
		return Cmiss_graphic_iso_surface_set_iso_scalar_field(reinterpret_cast<Cmiss_graphic_iso_surface_id>(id), field.getId());
	}

	int setIsoValues(int numberOfValues, double *values)
	{
		return Cmiss_graphic_iso_surface_set_iso_values(reinterpret_cast<Cmiss_graphic_iso_surface_id>(id),
			numberOfValues, values);
	}

	int setIsoRange(int numberOfValues, double firstValue, double lastValue)
	{
		return Cmiss_graphic_iso_surface_set_iso_range(reinterpret_cast<Cmiss_graphic_iso_surface_id>(id),
			numberOfValues, firstValue, lastValue);
	}

};

class GraphicLineAttributes
{
protected:
	Cmiss_graphic_line_attributes_id id;

public:

	// takes ownership of C handle, responsibility for destroying it
	explicit GraphicLineAttributes(Cmiss_graphic_line_attributes_id line_attributes_id) :
		id(line_attributes_id)
	{}

	GraphicLineAttributes(const GraphicLineAttributes& lineAttributes) :
		id(Cmiss_graphic_line_attributes_access(lineAttributes.id))
	{}

	~GraphicLineAttributes()
	{
		Cmiss_graphic_line_attributes_destroy(&id);
	}

	bool isValid()
	{
		return (0 != id);
	}

	int getBaseSize(int number, double *baseSize)
	{
		return Cmiss_graphic_line_attributes_get_base_size(id, number, baseSize);
	}

	int setBaseSize(int number, const double *baseSize)
	{
		return Cmiss_graphic_line_attributes_set_base_size(id, number, baseSize);
	}

	Field getOrientationScaleField()
	{
		return Field(Cmiss_graphic_line_attributes_get_orientation_scale_field(id));
	}

	int setOrientationScaleField(Field& orientationScaleField)
	{
		return Cmiss_graphic_line_attributes_set_orientation_scale_field(id, orientationScaleField.getId());
	}

	int getScaleFactors(int number, double *scaleFactors)
	{
		return Cmiss_graphic_line_attributes_get_scale_factors(id, number, scaleFactors);
	}

	int setScaleFactors(int number, const double *scaleFactors)
	{
		return Cmiss_graphic_line_attributes_set_scale_factors(id, number, scaleFactors);
	}

};

inline GraphicLineAttributes Graphic::getLineAttributes()
{
	return GraphicLineAttributes(Cmiss_graphic_get_line_attributes(id));
}

class GraphicPointAttributes
{
protected:
	Cmiss_graphic_point_attributes_id id;

public:

	// takes ownership of C handle, responsibility for destroying it
	explicit GraphicPointAttributes(Cmiss_graphic_point_attributes_id point_attributes_id) :
		id(point_attributes_id)
	  {}

	GraphicPointAttributes(const GraphicPointAttributes& pointAttributes) :
		id(Cmiss_graphic_point_attributes_access(pointAttributes.id))
		{}

	~GraphicPointAttributes()
	{
		Cmiss_graphic_point_attributes_destroy(&id);
	}

	bool isValid()
	{
		return (0 != id);
	}

	GraphicsFont getFont()
	{
		return GraphicsFont(Cmiss_graphic_point_attributes_get_font(id));
	}

	int setFont(GraphicsFont& font)
	{
		return Cmiss_graphic_point_attributes_set_font(id, font.getId());
	}

	int setGlyphType(Graphic::GlyphType type)
	{
		return Cmiss_graphic_point_attributes_set_glyph_type(id,
			static_cast<Cmiss_graphics_glyph_type>(type));
	}

	Field getLabelField()
	{
		return Field(Cmiss_graphic_point_attributes_get_label_field(id));
	}

	int setLabelField(Field& labelField)
	{
		return Cmiss_graphic_point_attributes_set_label_field(id, labelField.getId());
	}

};

inline GraphicPointAttributes Graphic::getPointAttributes()
{
	return GraphicPointAttributes(Cmiss_graphic_get_point_attributes(id));
}

} // namespace Cmiss

#endif /* __ZN_CMISS_GRAPHIC_HPP__ */
