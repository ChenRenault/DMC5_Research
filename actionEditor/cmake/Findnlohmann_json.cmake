if (TARGET nlohmann_json)
    return()
endif()

set(_nlohmann_json_SourceDir ${PROJECT_ROOT_DIR}/external/nlohmann)
set(_nlohmann_json_BinaryDir ${CMAKE_BINARY_DIR}/external/nlohmann)

add_subdirectory(${_nlohmann_json_SourceDir} ${_nlohmann_json_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    nlohmann_json
    REQUIRED_VARS
        _nlohmann_json_SourceDir
)

