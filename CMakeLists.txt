# check the minimum version
cmake_minimum_required( VERSION 2.8.12 )


# the project name
project( DASHBOARD )

################################
#### Option section
################################


################################
#### General configure section
################################

# force the Release build if not already set
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# setting common c++ flags
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

# setting debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g3 -O0")

# setting release with debug info flags
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -march=native -mtune=native -g3 -O2")

# setting release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -O3")



###############################################
##             EXTERNAL LIB                  ##
###############################################

# look for OpenCV
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/opencv/install/share/OpenCV")
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})


###############################################
##             BUILD SECTION                 ##
###############################################

# Set the application name
set( DASHBOARD_NAME "dashboard")

# Add the include folder
set( HDR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src )
include_directories( ${HDR_PATH} )


# Build the miniapp executable
set(SRC_FILES
	${SRC_PATH}/window.cc
	${SRC_PATH}/process_communicator.cc
	${SRC_PATH}/gnuplot_api.cc
	${SRC_PATH}/plot_description.cc
	${SRC_PATH}/plotter.cc
	${SRC_PATH}/consumer.cc
	${SRC_PATH}/config.cc
	)

set(HDR_FILES
	${HDR_PATH}/dashboard/window.hpp
	${HDR_PATH}/dashboard/render.hpp
	${HDR_PATH}/dashboard/process_communicator.hpp
	${HDR_PATH}/dashboard/gnuplot_api.hpp
	${HDR_PATH}/dashboard/string_utility.hpp
	${HDR_PATH}/dashboard/plot_description.hpp
	${HDR_PATH}/dashboard/plotter.hpp
  ${HDR_PATH}/dashboard/consumer.hpp
	${HDR_PATH}/dashboard/SynchronizedQueue.hpp
	${HDR_PATH}/dashboard/myMosquitto.hpp
	${HDR_PATH}/dashboard/config.hpp
	)


add_library( ${DASHBOARD_NAME} ${SRC_FILES} ${HDR_FILES} )
set_target_properties( ${DASHBOARD_NAME} PROPERTIES PUBLIC_HEADER "${HDR_FILES}" )

add_executable( ${DASHBOARD_NAME}_demo ${SRC_PATH}/main.cc )

# linking boost and mosquitto libraries
target_link_libraries( ${DASHBOARD_NAME}_demo boost_system boost_thread mosquitto mosquittopp boost_program_options)

target_link_libraries(${DASHBOARD_NAME}_demo ${DASHBOARD_NAME} ${OpenCV_LIBS})


# link the library with the OpenCV library
target_link_libraries(${DASHBOARD_NAME} ${OpenCV_LIBS} pthread)


if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (
		CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}"
		CACHE PATH "default install path"
	FORCE )
endif()

#install( TARGETS ${DASHBOARD_NAME} EXPORT dashboard-targets ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include )
#install( TARGETS ${DASHBOARD_NAME}_demo DESTINATION bin )
#install( EXPORT dashboard-targets DESTINATION cmake/dashboard-targets )



###############################################
##           EXPORT INFORMATION              ##
###############################################

if ( NOT (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME}) )
	set( DASHBOARD_LIB_NAME ${DASHBOARD_NAME} PARENT_SCOPE )
	set( DASHBOARD_HEADER_PATHS ${HDR_PATH} ${OpenCV_INCLUDE_DIRS} PARENT_SCOPE )
	set( DASHBOARD_LIB_PATHS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_INSTALL_PREFIX}/lib ${OpenCV_INSTALL_PATH}/lib PARENT_SCOPE )
	set( DASHBOARD_LIBRARIES ${DASHBOARD_NAME} ${OpenCV_LIBS} pthread PARENT_SCOPE )
endif(  NOT (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME}) )
#export( TARGETS ${DASHBOARD_NAME} FILE "dashboard_export.cmake" )


###############################################
##           PRINT INFORMATION               ##
###############################################

# configure the path for the current project
configure_file(
  "${PROJECT_SOURCE_DIR}/scripts/clang_autocomplete.in"
  "${PROJECT_SOURCE_DIR}/.clang_complete"
  @ONLY
  )

if ( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} )
	message ( STATUS "" )
	message ( STATUS "*********************************" )
	message ( STATUS "**        CONFIG STATUS        **" )
	message ( STATUS "*********************************" )
	message ( STATUS "Install prefix..................: ${CMAKE_INSTALL_PREFIX}" )
endif ( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} )
