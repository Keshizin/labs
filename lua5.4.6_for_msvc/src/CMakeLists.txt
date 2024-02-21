
#
# LUA LIBRARY
#

add_library(lua_msvc lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lcorolib.c ldblib.c liolib.c lmathlib.c loadlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c linit.c)

#
# LUA INTERPRETER
#

add_executable(lua lua.c)
target_link_libraries(lua PUBLIC lua_msvc)

#
# LUA COMPILER
#

add_executable(luac luac.c)
target_link_libraries(luac PUBLIC lua_msvc)

target_include_directories(lua_msvc PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
)