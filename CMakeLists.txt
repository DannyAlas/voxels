cmake_minimum_required(VERSION 3.10)
project(voxel-thing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
file(GLOB_RECURSE sources "${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB imgui_sources "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/*.cpp" "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/*.h")
file(GLOB imgui_impl_sources "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/backends/imgui_impl_glfw.cpp" 
      "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/backends/imgui_impl_glfw.h" 
      "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/backends/imgui_impl_opengl3.cpp" 
      "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/backends/imgui_impl_opengl3.h"
)

add_library(ImGui ${imgui_sources} ${imgui_impl_sources})
target_include_directories(ImGui PUBLIC "${PROJECT_SOURCE_DIR}/lib/vendor/imgui" "${PROJECT_SOURCE_DIR}/lib/vendor/imgui/backends")


add_definitions(-DSHDEBUG)

add_executable(voxel-thing ${sources} src/glad.c)

target_link_libraries(voxel-thing glfw PkgConfig::freetype2 ImGui)
