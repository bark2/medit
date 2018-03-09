cmake_minimum_required (VERSION 2.6)
project (medit)
set(CMAKE_BUILD_TYPE Debug)

add_executable(medit src/glad.c src/gapbuffer.cc src/medit.cc src/shader.cc src/text_renderer.cc)

include_directories(include)

find_package(PkgConfig)
pkg_check_modules(MEDIT REQUIRED freetype2 glfw3)

target_link_libraries(medit stdc++ freetype dl)
target_link_libraries(medit ${MEDIT_LIBRARIES})
target_include_directories(medit PUBLIC ${MEDIT_INCLUDE_DIRS})
target_compile_options(medit PUBLIC ${MEDIT_CFLAGS_OTHER})
