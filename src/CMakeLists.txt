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
if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
  string(REPLACE "/RTC1" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

#if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
  #string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#endif()
set( CMAKE_CXX_FLAGS "/clr /EHa /AI${IntersonArraySDK_DIR}/Libraries" )
set( COMPILE_FLAGS "/clr /EHa /AI${IntersonArraySDK_DIR}/Libraries" )

set( IntersonArrayCxx_SRCS
  IntersonArrayCxxControlsHWControls.cxx
  IntersonArrayCxxImagingContainer.cxx
  IntersonArrayCxxIntersonClass.cxx
  )

include( GenerateExportHeader )
add_library( IntersonArrayCxx SHARED
  ${IntersonArrayCxx_SRCS} )

generate_export_header( IntersonArrayCxx
  BASE_NAME IntersonArrayCxx
  EXPORT_MACRO_NAME IntersonArrayCxx_EXPORT
  EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/IntersonArrayCxx_Export.h
  )
#set_target_properties( IntersonArrayCxx PROPERTIES
  #CMAKE_CXX_FLAGS "/clr /EHa /AI${IntersonArraySDK_DIR}/Libraries"
  #)
  # COMPILE_FLAGS "/clr /EHa /AI${IntersonArraySDK_DIR}/Libraries"

install( TARGETS IntersonArrayCxx
  EXPORT IntersonArraySDKCxxTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT Runtime
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT Development
  )
if( BUILD_TESTING )
  # For running tests
  add_custom_command( TARGET IntersonArrayCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/bin"
    )
endif()
if( BUILD_APPLICATIONS )
  add_custom_command( TARGET IntersonArrayCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/app/bin"
    )
  add_custom_command( TARGET IntersonArrayCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_BINARY_DIR}/bin/IntersonArrayCxx.dll" "${PROJECT_BINARY_DIR}/app/bin"
    )
endif()
