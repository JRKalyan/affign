cmake_minimum_required(VERSION 3.0)

project(affign)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources)

set(HEADER_LIST ${HEADER_DIR}/affignmain.h 
	${HEADER_DIR}/affignaligner.h 
	${HEADER_DIR}/affignexcept.h)
	
set(RESOURCE_LIST ${RESOURCE_DIR}/licenseMIT.txt
	${RESOURCE_DIR}/licenseOPENCV.txt
	${RESOURCE_DIR}/licenseDLIB.txt
	${RESOURCE_DIR}/affignlogo.xpm)
	
set(SRC_LIST affignmain.cpp 
	affignaligner.cpp
	affignexcept.cpp)

set(COMPILE_LIST ${SRC_LIST} ${HEADER_LIST} ${RESOURCE_LIST})

include_directories(${RESOURCE_DIR} ${HEADER_DIR})

if (WIN32)
	set(COMPILE_LIST ${COMPILE_LIST} ${RESOURCE_DIR}/affignresources.rc)
	add_executable(${PROJECT_NAME} WIN32 ${COMPILE_LIST})
elseif (UNIX)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
elseif (APPLE)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
else (WIN32)
	add_executable(${PROJECT_NAME} ${COMPILE_LIST})
endif (WIN32)

#### External Dependencies ##########################################

# WXWIDGETS
if (WIN32)
	set(wxWidgets_CONFIGURATION mswu)
endif (WIN32)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

# OPENCV
find_package(OpenCV REQUIRED)

# DLIB
find_path(DLIB_DIR dlib ${CMAKE_CURRENT_SOURCE_DIR}/external/dlib)

if ((NOT DLIB_DIR) OR (NOT EXISTS ${DLIB_DIR}))
	if (NOT EXISTS ${DLIB_DIR})
		execute_process(COMMAND ${CMAKE_COMMAND} 
			-E make_directory ${DLIB_DIR})
	endif (NOT EXISTS ${DLIB_DIR})
	message(STATUS "Retrieving DLIB")
	execute_process(COMMAND git submodule update 
		--init external/dlib
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	find_file(TEST_CMAKEFILE CMakeLists.txt ${DLIB_DIR})
	if ((NOT TEST_CMAKEFILE) OR (NOT EXISTS TEST_CMAKEFILE))
		# The git command probably didn't work - do it manually
		message(FATAL_ERROR "ABORTING: dlib not properly updated")
	endif ((NOT TEST_CMAKEFILE) OR (NOT EXISTS TEST_CMAKEFILE))
endif ((NOT DLIB_DIR) OR (NOT EXISTS ${DLIB_DIR}))

add_subdirectory(external/dlib dlib_build)

# Linking
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} dlib::dlib)

#####################################################################