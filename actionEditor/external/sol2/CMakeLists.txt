project(sol2)

add_library(sol2 INTERFACE)

set(_sol2_Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/single/include/sol/sol.hpp
)

target_include_directories(sol2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sol2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/single/include)
target_sources(sol2 INTERFACE ${_sol2_Sources})

source_group("ThirdParty\\sol2" FILES ${_sol2_Sources})