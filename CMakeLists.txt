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

cmake_minimum_required( VERSION 3.6 )

project( IntersonArraySDKCxx )

# CMake modules
include( CMakePackageConfigHelpers )
include( CTest )

# Options
option( BUILD_APPLICATIONS "Build applications" OFF )

# External dependencies
find_path( IntersonArraySDK_DIR
  NAMES Libraries/IntersonArray.dll
  PATHS C:/IntersonArraySDK
  )
if( NOT IntersonArraySDK_DIR )
  message( FATAL_ERROR "Please specify the path to the IntersonArraySDK"
    " in IntersonArraySDK_DIR" )
endif()

# Install relative directories
set( INSTALL_LIB_DIR lib )
set( INSTALL_BIN_DIR bin )
set( INSTALL_INCLUDE_DIR include )
set( INSTALL_CMAKE_DIR cmake )

# Output directories
if( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/${INSTALL_BIN_DIR} )
endif()
if( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/${INSTALL_LIB_DIR} )
endif()
if( NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${IntersonArraySDKCxx_BINARY_DIR}/${INSTALL_LIB_DIR} )
endif()

# Add subdirectories
add_subdirectory( include )
add_subdirectory( src )
if( BUILD_TESTING )
  add_subdirectory( test )
endif()
if( BUILD_APPLICATIONS )
  add_subdirectory( app )
endif()

# Configure 'IntersonArraySDKCxxConfig.cmake' for a build tree
set(CONFIG_DIR_CONFIG ${PROJECT_BINARY_DIR})
set(intersonArraySDKCxx_config ${PROJECT_BINARY_DIR}/IntersonArraySDKCxxConfig.cmake)
configure_package_config_file(
  IntersonArraySDKCxxConfig.cmake.in
  ${intersonArraySDKCxx_config}
  INSTALL_DESTINATION ${PROJECT_BINARY_DIR}
  PATH_VARS CONFIG_DIR_CONFIG
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
  
# Configure 'IntersonArraySDKCxxConfig.cmake' for an install tree
set(CONFIG_DIR_CONFIG ${INSTALL_CMAKE_DIR})
set( intersonArraySDKCxx_install_config ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/IntersonArraySDKCxxConfig.cmake )
configure_package_config_file(
  IntersonArraySDKCxxConfig.cmake.in
  ${intersonArraySDKCxx_install_config}
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}
  PATH_VARS CONFIG_DIR_CONFIG
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
install(
  FILES ${intersonArraySDKCxx_install_config}
  DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT Development
  )

# Configure 'IntersonArraySDKCxxTargets.cmake'
export( TARGETS IntersonArrayCxx
  FILE ${PROJECT_BINARY_DIR}/IntersonArraySDKCxxTargets.cmake
  )

# Install 'IntersonArraySDKCxxTargets.cmake' and 'IntersonArraySDKCxxConfig.cmake'
install( EXPORT IntersonArraySDKCxxTargets
  FILE IntersonArraySDKCxxTargets.cmake
  DESTINATION ${INSTALL_CMAKE_DIR}
  COMPONENT Development
  )
