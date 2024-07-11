project(nlohmann_json)

add_library(nlohmann_json INTERFACE)

set(_nlohmann_json_Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
)

target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(nlohmann_json INTERFACE ${_nlohmann_json_Sources})

source_group("ThirdParty\\nlohmann_json" FILES ${_nlohmann_json_Sources})