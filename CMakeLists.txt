cmake_minimum_required (VERSION 2.6)
project (medit)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(medit src/buffer.cc src/line.cc src/medit.cc src/render.cc)
add_executable(line_test src/line.cc src/line_test.cc)

find_package(PkgConfig)
#pkg_check_modules(MEDIT REQUIRED freetype2 glfw3)
pkg_check_modules(MEDIT REQUIRED ncurses)


target_link_libraries(medit ${MEDIT_LIBRARIES})
target_include_directories(medit PUBLIC ${MEDIT_INCLUDE_DIRS})
target_compile_options(medit PUBLIC ${MEDIT_CFLAGS_OTHER})
