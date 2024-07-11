if (TARGET sol2)
    return()
endif()

set(_sol2_SourceDir ${PROJECT_ROOT_DIR}/external/sol2)
set(_sol2_BinaryDir ${CMAKE_BINARY_DIR}/external/sol2)

add_subdirectory(${_sol2_SourceDir} ${_sol2_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    sol2
    REQUIRED_VARS
        _sol2_SourceDir
)

