cmake_minimum_required(VERSION 3.16)
project(blackhole)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(glfw3 REQUIRED)

# Add executable with multiple source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")
add_executable(blackhole ${SOURCES})

# Include directories
target_include_directories(blackhole PRIVATE
    ${GLFW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    "src/"
)

# Link libraries
target_link_libraries(blackhole PRIVATE
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    glfw
    ${CMAKE_DL_LIBS}
)

# Compiler flags for optimization and warnings
target_compile_options(blackhole PRIVATE
    -Wall -Wextra -O3 -march=native
    $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    $<$<CONFIG:Release>:-DNDEBUG>
)

# Copy shaders to build directory
file(GLOB SHADERS "shaders/*.vert" "shaders/*.frag" "shaders/*.comp")
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME} COPYONLY)
endforeach()
