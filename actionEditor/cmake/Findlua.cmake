if (TARGET lua)
    return()
endif()

set(_lua_SourceDir ${PROJECT_ROOT_DIR}/external/lua)
set(_lua_BinaryDir ${CMAKE_BINARY_DIR}/external/lua)

add_subdirectory(${_lua_SourceDir} ${_lua_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    lua
    REQUIRED_VARS
        _lua_SourceDir
)

