project(lua VERSION 5.4.3)

set(_lua_Sources
    src/lapi.c
    src/lapi.h
    src/lauxlib.c
    src/lauxlib.h
    src/lbaselib.c
    src/lcode.c
    src/lcode.h
    src/lcorolib.c
    src/lctype.c
    src/lctype.h
    src/ldblib.c
    src/ldebug.c
    src/ldebug.h
    src/ldo.c
    src/ldo.h
    src/ldump.c
    src/lfunc.c
    src/lfunc.h
    src/lgc.c
    src/lgc.h
    src/linit.c
    src/liolib.c
    src/ljumptab.h
    src/llex.c
    src/llex.h
    src/llimits.h
    src/lmathlib.c
    src/lmem.c
    src/lmem.h
    src/loadlib.c
    src/lobject.c
    src/lobject.h
    src/lopcodes.c
    src/lopcodes.h
    src/lopnames.h
    src/loslib.c
    src/lparser.c
    src/lparser.h
    src/lprefix.h
    src/lstate.c
    src/lstate.h
    src/lstring.c
    src/lstring.h
    src/lstrlib.c
    src/ltable.c
    src/ltable.h
    src/ltablib.c
    src/ltm.c
    src/ltm.h
    src/lua.h
    src/lua.hpp
    src/luaconf.h
    src/lualib.h
    src/lundump.c
    src/lundump.h
    src/lutf8lib.c
    src/lvm.c
    src/lvm.h
    src/lzio.c
    src/lzio.h
)

add_library(${PROJECT_NAME} STATIC ${_lua_Sources})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#target_compile_definitions(${PROJECT_NAME} PUBLIC "ImDrawIdx=unsigned int")
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_lua_Sources})

# if (WIN32)
# target_compile_definitions(lua PRIVATE LUA_USE_WINDOWS)
# endif()

# target_include_directories(lua PUBLIC lua)
# if (UNIX)
# target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
# target_link_libraries(lua PRIVATE dl)
# endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "external")

