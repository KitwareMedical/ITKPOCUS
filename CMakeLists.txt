##############################################################################
#
# Library:   IntersonArraySDK
#
# Copyright Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################
cmake_minimum_required( VERSION 2.8 )

project( IntersonArraySDKCxx )

if( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/bin )
endif()
if( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/lib )
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/lib )
endif()

set( CONF_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include )
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

find_path( IntersonArraySDK_DIR NAMES Libraries/IntersonArray.dll
  PATHS C:/IntersonArraySDK )
if( NOT IntersonArraySDK_DIR )
  message( SEND_ERROR "Please specify the path to the IntersonArraySDK"
    " in IntersonArraySDK_DIR" )
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

export( TARGETS IntersonArrayCxx
  FILE "${PROJECT_BINARY_DIR}/IntersonArraySDKCxxTargets.cmake" )
export( PACKAGE IntersonArraySDKCxx )

file( RELATIVE_PATH _relative_include_dir
  "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}" )

configure_file( IntersonArraySDKCxxConfig.cmake.in
  "${PROJECT_BINARY_DIR}/IntersonArraySDKCxxConfig.cmake" @ONLY )

set( CONF_INCLUDE_DIRS
  "\${IntersonArraySDKCxx_CMAKE_DIR}/${_relative_include_dir} ${CONF_INCLUDE_DIRS}" )
configure_file( IntersonArraySDKCxxConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/IntersonArraySDKCxxConfig.cmake"
  @ONLY )

install( FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/IntersonArraySDKCxxConfig.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT Development
  )
install( EXPORT IntersonArraySDKCxxTargets DESTINATION
  "${INSTALL_CMAKE_DIR}"
  COMPONENT Development
  )
