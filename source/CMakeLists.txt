cmake_minimum_required(VERSION 3.0)

project(affign)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCEU_DIR}/../resources)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file (GLOB HEADER_LIST ${HEADER_DIR}/*.h)
file (GLOB SOURCE_LIST ${SOURCE_DIR}/*.cpp)
file (GLOB RESOURCE_LIST ${RESOURCE_DIR}/*)

set(COMPILE_LIST ${SOURCE_LIST} ${HEADER_LIST} ${RESOURCE_LIST})

include_directories(${HEADER_DIR})

if (WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${COMPILE_LIST})
elseif (UNIX)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
elseif (APPLE)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
else (WIN32)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
endif (WIN32)

#### External Dependencies ##########################################
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
#####################################################################