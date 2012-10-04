
SET(APP_HDRS
    source/mesh/cmiss_element_private_app.hpp
    source/computed_field/computed_field_image_app.h
    source/computed_field/computed_field_integration_app.h
    source/computed_field/computed_field_alias_app.h
    source/computed_field/computed_field_coordinate_app.h
    source/computed_field/computed_field_scene_viewer_projection_app.h
    source/finite_element/finite_element_app.h
    source/finite_element/finite_element_app.h
    source/graphics/graphic_app.h
    source/graphics/rendition_app.h
    source/general/geometry_app.h
    source/general/enumerator_private_app.h
    source/general/enumerator_app.h
    source/computed_field/computed_field_private_app.hpp
    source/three_d_drawing/graphics_buffer_app.h
    source/image_processing/computed_field_sigmoid_image_filter_app.h
    source/image_processing/computed_field_mean_image_filter_app.h
    source/image_processing/computed_field_rescale_intensity_image_filter_app.h
    source/image_processing/computed_field_derivative_image_filter_app.h
    source/image_processing/computed_field_canny_edge_detection_filter_app.h
    source/image_processing/computed_field_curvature_anisotropic_diffusion_image_filter_app.h
    source/image_processing/computed_field_histogram_image_filter_app.h
    source/image_processing/computed_field_discrete_gaussian_image_filter_app.h
    source/image_processing/computed_field_connected_threshold_image_filter_app.h
    source/image_processing/computed_field_gradient_magnitude_recursive_gaussian_image_filter_app.h
    source/image_processing/computed_field_fast_marching_image_filter_app.h
    source/image_processing/computed_field_binary_erode_image_filter_app.h
    source/image_processing/computed_field_binary_dilate_image_filter_app.h
    source/computed_field/computed_field_time_app.h
    source/image_processing/computed_field_binary_threshold_image_filter_app.h
    source/image_processing/computed_field_threshold_image_filter_app.h
    source/image_processing/computed_field_image_resample_app.h
    source/computed_field/computed_field_string_constant_app.h
    source/computed_field/computed_field_deformation_app.h
    source/computed_field/computed_field_finite_element_app.h
    source/computed_field/computed_field_vector_operators_app.hpp
    source/computed_field/computed_field_matrix_operators_app.hpp
    source/computed_field/computed_field_nodeset_operators_app.hpp
    source/computed_field/computed_field_lookup_app.h
    source/computed_field/computed_field_logical_operators_app.h
    source/computed_field/computed_field_function_app.h
    source/computed_field/computed_field_fibres_app.h
    source/computed_field/computed_field_derivatives_app.h
    source/computed_field/computed_field_curve_app.h
    source/computed_field/computed_field_conditional_app.h
    source/computed_field/computed_field_composite_app.h
    source/computed_field/computed_field_compose_app.h
    source/computed_field/computed_field_format_output_app.h
    source/computed_field/computed_field_trigonometry_app.h
    source/computed_field/computed_field_arithmetic_operators_app.h
    source/minimise/minimise_app.h
    source/finite_element/export_finite_element_app.h
    source/graphics/element_point_ranges_app.h
    source/graphics/environment_map_app.h
    source/graphics/rendition_app.h
    source/finite_element/finite_element_region_app.h
    source/graphics/scene_viewer_app.h
    source/graphics/graphics_object_app.h
    source/graphics/tessellation_app.hpp
    source/graphics/tessellation_app.hpp
    source/graphics/graphics_filter_app.hpp
    source/computed_field/computed_field_app.h
    source/curve/curve_app.h
    source/graphics/render_to_finite_elements_app.h
    source/graphics/render_to_finite_elements_app.h
    source/graphics/auxiliary_graphics_types_app.h
    source/finite_element/finite_element_conversion_app.h
    source/graphics/texture_app.h
    source/graphics/colour_app.h
    source/graphics/scene_app.h
    source/graphics/spectrum_settings_app.h
    source/graphics/light_model_app.h
    source/graphics/light_app.h
    source/computed_field/computed_field_set_app.h
    source/general/multi_range_app.h
    source/choose/choose_class.hpp
    source/choose/choose_enumerator_class.hpp
    source/choose/choose_listbox_class.hpp
    source/choose/text_FE_choose_class.hpp
    source/choose/text_choose_from_fe_element.hpp
    source/choose/choose_manager_class.hpp
    source/choose/choose_list_class.hpp
    source/choose/choose_manager_listbox_class.hpp
    source/api/cmiss_fdio.h
    source/api/cmiss_idle.h
    source/comfile/comfile.h
    source/command/cmiss.h
    source/command/command.h
    source/command/console.h
    source/command/example_path.h
    source/command/parser.h
    source/context/context_app.h
    source/context/user_interface_module.h
    source/element/element_point_tool.h
    source/element/element_tool.h
    source/element/element_point_viewer_wx.h
    source/emoter/emoter_dialog.h
    source/finite_element/read_fieldml_01.h
    source/finite_element/write_fieldml_01.h
    source/graphics/graphics_window.h
    source/graphics/graphics_window_private.hpp
    source/graphics/texturemap.h
    source/graphics/transform_tool.h
    source/dialog/tessellation_dialog.hpp
    source/graphics/region_tree_viewer_wx.h
    source/graphics/spectrum_editor_wx.h
    source/graphics/spectrum_editor_dialog_wx.h
    source/graphics/material_app.h
    source/graphics/spectrum_app.h
    source/interaction/interactive_tool.h
    source/interaction/interactive_tool_private.h
    source/io_devices/matrix.h
    source/region/cmiss_region_app.h
    source/node/node_tool.h
    source/three_d_drawing/window_system_extensions.h
    source/colour/colour_editor_wx.hpp
    source/comfile/comfile_window_wx.h
    source/comfile/comfile_window_wx.hpp
    source/command/command_window.h
    source/material/material_editor_wx.h
    source/node/node_viewer_wx.h
    source/transformation/transformation_editor_wx.hpp
    source/user_interface/confirmation.h
    source/user_interface/event_dispatcher.h
    source/user_interface/fd_io.h
    source/user_interface/filedir.h
    source/user_interface/idle.h
    source/user_interface/process_list_or_write_command.hpp
    source/user_interface/user_interface.h
    source/user_interface/user_interface_wx.hpp)

