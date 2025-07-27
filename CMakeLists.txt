cmake_minimum_required(VERSION 3.16)
project(qoi_rw C CXX)

if(WIN32)
find_package(SDL3 REQUIRED)
endif()

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in SDL
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(qoi_rw)

target_sources(qoi_rw
PUBLIC qoi_header.cpp
PUBLIC qoi_reader.cpp
PUBLIC main.cpp)

# Link to the actual SDL3 library.
target_link_libraries(qoi_rw PRIVATE SDL3::SDL3)
