cmake_minimum_required(VERSION 3.15)

# Required for setting MSVC CRT properly (see below)
cmake_policy(SET CMP0091 NEW)

project(affign)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file (GLOB HEADER_LIST ${HEADER_DIR}/*.h)
file (GLOB SOURCE_LIST ${SOURCE_DIR}/*.cpp)
file (GLOB RESOURCE_LIST ${RESOURCE_DIR}/*)

set(COMPILE_LIST ${SOURCE_LIST} ${HEADER_LIST} ${RESOURCE_LIST})

include_directories(${HEADER_DIR})

add_executable(${PROJECT_NAME} ${COMPILE_LIST})

# Using filesystem requires C++17
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
# Ensure when using MSVC to use statically linked CRT
# (The generator expression will include "Debug" on debug configuration)
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY 
    "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#### External Dependencies ##########################################

# NOTE: 
# - When building OpenCV, you have to set BUILD_SHARED_LIBS to false
#   to link statically.
# - Point Cmake to the opencv build directory (set OpenCV_DIR)
# - Make sure you generate for the same target platform as the OpenCV
#   build targets

find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
#####################################################################