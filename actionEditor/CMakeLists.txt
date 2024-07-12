cmake_minimum_required(VERSION 3.12)

project(action-editor)

# Define PROJECT_ROOT_DIR pointing to project root directory
get_filename_component(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR} ABSOLUTE CACHE)

# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Point CMake where to look for module files.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_ROOT_DIR}/cmake)

# Node editor use C++14
set(CMAKE_CXX_STANDARD            23)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

# Macro that will configure an application
macro(add_app_executable name)
    project(${name})

    set(_APP_Sources
        ${ARGN}
    )

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_APP_Sources})

    file(GLOB _APP_CommonResources CONFIGURE_DEPENDS "${PROJECT_ROOT_DIR}/source/data/*")
    file(GLOB _APP_Resources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")

    set(_APP_Type)
    if (WIN32)
        set(_APP_Type WIN32)

        set(ApplicationIcon ${PROJECT_ROOT_DIR}/source/Application/Support/Icon.ico)
        file(TO_NATIVE_PATH "${ApplicationIcon}" ApplicationIcon)
        string(REPLACE "\\" "\\\\" ApplicationIcon "${ApplicationIcon}")
        configure_file(
            ${PROJECT_ROOT_DIR}/source/Application/Support/Resource.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
        )
        source_group(TREE "${PROJECT_ROOT_DIR}/source" FILES ${_APP_CommonResources})
        source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${_APP_Resources})
        list(APPEND _APP_Resources
            ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
            ${_APP_CommonResources}
        )
        source_group("resources" FILES ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc)
    elseif (APPLE)
        set(_APP_Type MACOSX_BUNDLE)

        set_source_files_properties(${_APP_Resources} ${_APP_CommonResources} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/data"
        )
        set(_APP_Icon "${PROJECT_ROOT_DIR}/source/application/support/Icon.icns")
        list(APPEND _APP_Resources ${_APP_Icon})
        set_source_files_properties(${_APP_Icon} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
    endif()

    add_executable(${name} ${_APP_Type} ${_APP_Sources} ${_APP_Resources} ${_APP_CommonResources})
    
    find_package(fmt CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(spdlog CONFIG REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(lua CONFIG REQUIRED)
    find_package(sol2 CONFIG REQUIRED)
    find_package(imgui REQUIRED)
    find_package(imgui_node_editor REQUIRED)

    target_link_libraries(${name} PRIVATE
        fmt::fmt
        glm::glm
        nlohmann_json::nlohmann_json
        imgui
        imgui_node_editor
        lua
        spdlog::spdlog
        sol2
        application
    )
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)

    target_compile_options(${name} 
        PRIVATE 
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    )

    set(_APPBinDir ${CMAKE_BINARY_DIR}/bin)

    set_target_properties(${name} PROPERTIES
        FOLDER "source"
        RUNTIME_OUTPUT_DIRECTORY                "${_APPBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${_APPBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_APPBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${_APPBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${_APPBinDir}"
        DEBUG_POSTFIX                           _d
        RELWITHDEBINGO_POSTFIX                  _rd
        MINSIZEREL_POSTFIX                      _r
        VS_DEBUGGER_WORKING_DIRECTORY           ${_APPBinDir}
        MACOSX_BUNDLE_INFO_PLIST                "${PROJECT_ROOT_DIR}/source/application/support/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME               "${PACKAGE_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER            "com.sandbox.collisions"
        MACOSX_BUNDLE_LONG_VERSION_STRING       "${PACKAGE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING      "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE                 Icon.icns
    )

    add_custom_command(
        TARGET ${name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${_APPBinDir}/data
    )

    set(_ResourceRoot ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(_Resource ROOT "${PROJECT_ROOT_DIR}/source/data" ${_APP_CommonResources} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${_APP_Resources})
        if (_Resource STREQUAL ROOT)
            set(_ResourceRoot FALSE)
            continue()
        elseif(NOT _ResourceRoot)
            set(_ResourceRoot ${_Resource})
            continue()
        endif()

        if ("${_Resource}" MATCHES "\.DS_Store$")
            list(REMOVE_ITEM _APP_Resources ${_Resource})
            list(REMOVE_ITEM _APP_CommonResources ${_Resource})
            continue()
        endif()

        file(RELATIVE_PATH _RelResource ${_ResourceRoot} ${_Resource})

        add_custom_command(
            TARGET ${name}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Resource} ${_APPBinDir}/data/${_RelResource}
        )
    endforeach()

endmacro()

add_subdirectory(source/application)

add_app_executable(actionEditor
    source/bindings/FS.hpp
    source/bindings/FS.cpp
    source/bindings/Imgui.hpp
    source/bindings/Imgui.cpp
    source/bindings/Json.hpp
    source/bindings/Json.cpp
    source/Math.hpp
    source/actionEditor.cpp
)
