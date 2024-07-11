if (TARGET imgui_node_editor)
    return()
endif()

set(_imgui_node_editor_SourceDir ${PROJECT_ROOT_DIR}/external/imgui-node-editor)
set(_imgui_node_editor_BinaryDir ${CMAKE_BINARY_DIR}/external/imgui-node-editor)

add_subdirectory(${_imgui_node_editor_SourceDir} ${_imgui_node_editor_BinaryDir})

source_group(TREE ${PROJECT_ROOT_DIR} FILES ${_imgui_node_editor_Sources})


include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_node_editor
    REQUIRED_VARS
        PROJECT_ROOT_DIR
)
