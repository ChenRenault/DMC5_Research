if (TARGET glm)
    return()
endif()

set(_glm_SourceDir ${PROJECT_ROOT_DIR}/external/glm)
set(_glm_BinaryDir ${CMAKE_BINARY_DIR}/external/glm)

add_subdirectory(${_glm_SourceDir} ${_glm_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    glm
    REQUIRED_VARS
        _glm_SourceDir
)

