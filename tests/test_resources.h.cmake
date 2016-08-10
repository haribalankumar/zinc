
#ifndef TEST_RESOURCES_H_
#define TEST_RESOURCES_H_

class TestResources
{

public:

	enum ResourcesName
	{
		IMAGE_PNG_RESOURCE = 1,
		IMAGE_ANALYZE_BIGENDIAN_RESOURCE = 2,
		IMAGE_ANALYZE_LITTLEENDIAN_RESOURCE = 3,
		IMAGE_ANALYZE_LITTLEENDIAN_TAR_RESOURCE = 4,
		FIELDMODULE_EXNODE_RESOURCE = 5,
		FIELDMODULE_CUBE_RESOURCE = 6,
		FIELDMODULE_CUBE_GRID_RESOURCE = 7,
		FIELDMODULE_REGION_INPUT_RESOURCE = 8,
		FIELDMODULE_EMBEDDING_ISSUE3614_RESOURCE = 9,
		TESTIMAGE_GRAY_JPG_RESOURCE = 10,
		FIELDIMAGE_BLOCKCOLOURS_RESOURCE = 11,
		HEART_EXNODE_GZ = 12,
		HEART_EXELEM_GZ = 13,
		FIELDMODULE_TWO_CUBES_RESOURCE = 14,
		FIELDMODULE_ALLSHAPES_RESOURCE = 15,
		OPTIMISATION_CUBE_TRICUBIC_LAGRANGE_RESOURCE = 16,
		FIELDMODULE_CUBE_XYZP_RESOURCE = 17,
		FIELDIO_FIELDML_CUBE_RESOURCE = 18,
		FIELDIO_FIELDML_TETMESH_RESOURCE = 19,
		FIELDIO_FIELDML_WHEEL_DIRECT_RESOURCE = 20,
		FIELDIO_FIELDML_WHEEL_INDIRECT_RESOURCE = 21,
		FIELDIO_EX_LINES_UNIT_SCALE_FACTORS_RESOURCE = 22,
		FIELDIO_EX_LINES_ALTERNATE_NODE_ORDER_RESOURCE = 23,
		FIELDIO_EX_LINES_INCONSISTENT_NODE_ORDER_RESOURCE = 24,
		FIELDIO_EX_HERMITE_FIGURE8_RESOURCE = 25,
		FIELDIO_EX_TWOHERMITECUBES_NOSCALEFACTORS_RESOURCE = 26,
		SCENEVIEWER_DESCRIPTION_JSON_RESOURCE = 27,
		SPECTRUM_DESCRIPTION_JSON_RESOURCE = 28,
		TESSELLATION_DESCRIPTION_JSON_RESOURCE = 29,
		TIMEKEEPER_DESCRIPTION_JSON_RESOURCE = 30,
		GRAPHICS_LINE_DESCRIPTION_JSON_RESOURCE = 31,
		GRAPHICS_STREAMLINES_DESCRIPTION_JSON_RESOURCE = 32,
		GRAPHICS_POINTS_DESCRIPTION_JSON_RESOURCE = 33,
		FIELDMODULE_CUBESQUARELINE_RESOURCE = 34,
		REGION_INCORRECT_RESOURCE = 35,
		FIELDMODULE_DESCRIPTION_JSON_RESOURCE = 36,
		FIELDMODULE_CUBE_TRICUBIC_DEFORMED_RESOURCE = 37,
		FIELDMODULE_HEART_SURFACE_RESOURCE = 38,
		FIELDMODULE_PLATE_600X300_RESOURCE = 39
	};

	TestResources()
	{  }

	~TestResources()
	{  }

	static const char *getLocation(ResourcesName resourceName)
	{
		if (resourceName == TestResources::IMAGE_PNG_RESOURCE)
		{
			return "@IMAGE_PNG_RESOURCE@";
		}
		if (resourceName == TestResources::IMAGE_ANALYZE_BIGENDIAN_RESOURCE)
		{
			return "@IMAGE_ANALYZE_BIGENDIAN_RESOURCE@";
		}
		if (resourceName == TestResources::IMAGE_ANALYZE_LITTLEENDIAN_RESOURCE)
		{
			return "@IMAGE_ANALYZE_LITTLEENDIAN_RESOURCE@";
		}
		if (resourceName == TestResources::IMAGE_ANALYZE_LITTLEENDIAN_TAR_RESOURCE)
		{
			return "@IMAGE_ANALYZE_LITTLEENDIAN_TAR_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_EXNODE_RESOURCE)
		{
			return "@FIELDMODULE_EXNODE_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_CUBE_RESOURCE)
		{
			return "@FIELDMODULE_CUBE_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_CUBE_GRID_RESOURCE)
		{
			return "@FIELDMODULE_CUBE_GRID_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_REGION_INPUT_RESOURCE)
		{
			return "@FIELDMODULE_REGION_INPUT_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_EMBEDDING_ISSUE3614_RESOURCE)
		{
			return "@FIELDMODULE_EMBEDDING_ISSUE3614_RESOURCE@";
		}
		if (resourceName == TestResources::TESTIMAGE_GRAY_JPG_RESOURCE)
		{
			return "@TESTIMAGE_GRAY_JPG_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIMAGE_BLOCKCOLOURS_RESOURCE)
		{
			return "@FIELDIMAGE_BLOCKCOLOURS_RESOURCE@";
		}
		if (resourceName == TestResources::HEART_EXNODE_GZ)
		{
			return "@HEART_EXNODE_GZ@";
		}
		if (resourceName == TestResources::HEART_EXELEM_GZ)
		{
			return "@HEART_EXELEM_GZ@";
		}
		if (resourceName == TestResources::FIELDMODULE_TWO_CUBES_RESOURCE)
		{
			return "@FIELDMODULE_TWO_CUBES_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_ALLSHAPES_RESOURCE)
		{
			return "@FIELDMODULE_ALLSHAPES_RESOURCE@";
		}
		if (resourceName == TestResources::OPTIMISATION_CUBE_TRICUBIC_LAGRANGE_RESOURCE)
		{
			return "@OPTIMISATION_CUBE_TRICUBIC_LAGRANGE_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_CUBE_XYZP_RESOURCE)
		{
			return "@FIELDMODULE_CUBE_XYZP_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_FIELDML_CUBE_RESOURCE)
		{
			return "@FIELDIO_FIELDML_CUBE_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_FIELDML_TETMESH_RESOURCE)
		{
			return "@FIELDIO_FIELDML_TETMESH_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_FIELDML_WHEEL_DIRECT_RESOURCE)
		{
			return "@FIELDIO_FIELDML_WHEEL_DIRECT_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_FIELDML_WHEEL_INDIRECT_RESOURCE)
		{
			return "@FIELDIO_FIELDML_WHEEL_INDIRECT_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_EX_LINES_UNIT_SCALE_FACTORS_RESOURCE)
		{
			return "@FIELDIO_EX_LINES_UNIT_SCALE_FACTORS_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_EX_LINES_ALTERNATE_NODE_ORDER_RESOURCE)
		{
			return "@FIELDIO_EX_LINES_ALTERNATE_NODE_ORDER_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_EX_LINES_INCONSISTENT_NODE_ORDER_RESOURCE)
		{
			return "@FIELDIO_EX_LINES_INCONSISTENT_NODE_ORDER_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_EX_HERMITE_FIGURE8_RESOURCE)
		{
			return "@FIELDIO_EX_HERMITE_FIGURE8_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDIO_EX_TWOHERMITECUBES_NOSCALEFACTORS_RESOURCE)
		{
			return "@FIELDIO_EX_TWOHERMITECUBES_NOSCALEFACTORS_RESOURCE@";
		}
		if (resourceName == TestResources::SCENEVIEWER_DESCRIPTION_JSON_RESOURCE)
		{
			return "@SCENEVIEWER_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == TestResources::SPECTRUM_DESCRIPTION_JSON_RESOURCE)
		{
			return "@SPECTRUM_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == TestResources::TESSELLATION_DESCRIPTION_JSON_RESOURCE)
		{
			return "@TESSELLATION_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == TestResources::TIMEKEEPER_DESCRIPTION_JSON_RESOURCE)
		{
			return "@TIMEKEEPER_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == GRAPHICS_LINE_DESCRIPTION_JSON_RESOURCE)
				{
			return "@GRAPHICS_LINE_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == GRAPHICS_STREAMLINES_DESCRIPTION_JSON_RESOURCE)
		{
			return "@GRAPHICS_STREAMLINES_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == GRAPHICS_POINTS_DESCRIPTION_JSON_RESOURCE)
		{
			return "@GRAPHICS_POINTS_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_CUBESQUARELINE_RESOURCE)
		{
			return "@FIELDMODULE_CUBESQUARELINE_RESOURCE@";
		}
		if (resourceName == TestResources::REGION_INCORRECT_RESOURCE)
		{
			return "@REGION_INCORRECT_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_DESCRIPTION_JSON_RESOURCE)
		{
			return "@FIELDMODULE_DESCRIPTION_JSON_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_CUBE_TRICUBIC_DEFORMED_RESOURCE)
		{
			return "@FIELDMODULE_CUBE_TRICUBIC_DEFORMED_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_HEART_SURFACE_RESOURCE)
		{
			return "@FIELDMODULE_HEART_SURFACE_RESOURCE@";
		}
		if (resourceName == TestResources::FIELDMODULE_PLATE_600X300_RESOURCE)
		{
			return "@FIELDMODULE_PLATE_600X300_RESOURCE@";
		}

		return 0;
	}
};

#endif /* TEST_RESOURCES_H_ */

