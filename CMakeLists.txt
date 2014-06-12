cmake_minimum_required( VERSION 2.8 )

project( IntersonSDKCxx )

if( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IntersonSDKCxx_BINARY_DIR}/bin )
endif()
if( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${IntersonSDKCxx_BINARY_DIR}/lib )
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${IntersonSDKCxx_BINARY_DIR}/lib )
endif()

set( CONF_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include
  )
include_directories( ${CONF_INCLUDE_DIRS} )
set( INSTALL_LIB_DIR lib )
set( INSTALL_BIN_DIR bin )
set( INSTALL_INCLUDE_DIR include )
if( WIN32 AND NOT CYGWIN )
  set( DEF_INSTALL_CMAKE_DIR CMake )
else()
  set( DEF_INSTALL_CMAKE_DIR lib/CMake/FooBar )
endif()
set( INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} )
foreach( path LIB BIN INCLUDE CMAKE )
  set( var INSTALL_${path}_DIR )
  if( NOT IS_ABSOLUTE "${${var}}" )
    set( ${var} "${CMAKE_INSTALL_PREFIX}/${${var}}" )
  endif()
endforeach()

find_path( IntersonSDK_DIR NAMES Libraries/Interson.dll
  PATHS C:/IntersonSDK )
if( NOT IntersonSDK_DIR )
  message( SEND_ERROR "Please specify the path to the IntersonSDK in
  IntersonSDK_DIR" )
endif()

add_subdirectory( include )
add_subdirectory( src )

include( CTest )
if( BUILD_TESTING )
  enable_testing()
  add_subdirectory( test )
endif()

option( BUILD_APPLICATIONS "Build applications" OFF )
if( BUILD_APPLICATIONS )
  add_subdirectory( app )
endif()

export( TARGETS IntersonCxx
  FILE "${PROJECT_BINARY_DIR}/IntersonSDKCxxTargets.cmake"
  )
export( PACKAGE IntersonSDKCxx )
file( RELATIVE_PATH _relative_include_dir
  "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}" )
configure_file( IntersonSDKCxxConfig.cmake.in
  "${PROJECT_BINARY_DIR}/IntersonSDKCxxConfig.cmake" @ONLY )
set( CONF_INCLUDE_DIRS "\${IntersonSDKCxx_CMAKE_DIR}/${_relative_include_dir}"
)
configure_file( IntersonSDKCxxConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/IntersonSDKCxxConfig.cmake" @ONLY )

install( FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/IntersonSDKCxxConfig.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT Development
  )
install( EXPORT IntersonSDKCxxTargets DESTINATION
  "${INSTALL_CMAKE_DIR}"
  COMPONENT Development
  )
