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

# XXX (Maeliss, Jc) Ideally, these flags should probably be associated with the
#                   targets using target_compile_options command.
if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
  string(REPLACE "/RTC1" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

#if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
  #string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#endif()
set( CMAKE_CXX_FLAGS "/clr /EHa /AI${IntersonArraySDK_DIR}/Libraries" )

set( IntersonArrayCxx_SRCS
  IntersonArrayCxxControlsHWControls.cxx
  IntersonArrayCxxImagingContainer.cxx
  IntersonArrayCxxIntersonClass.cxx
  )

add_library( IntersonArrayCxx SHARED
  ${IntersonArrayCxx_SRCS} )

target_include_directories( IntersonArrayCxx PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

find_library(MSCOREE_LIBRARY
  NAMES MSCOREE mscoree
  PATHS "C:/Program Files (x86)/Common Files/Microsoft/Visual C++ for Python/9.0/WinSDK/Lib/"
  )
if(MSCOREE_LIBRARY)
  target_link_libraries(IntersonArrayCxx ${MSCOREE_LIBRARY})
endif()

# Generate and install export header
include( GenerateExportHeader )
set(export_header ${PROJECT_BINARY_DIR}/include/IntersonArrayCxx_Export.h)
generate_export_header( IntersonArrayCxx
  BASE_NAME IntersonArrayCxx
  EXPORT_MACRO_NAME IntersonArrayCxx_EXPORT
  EXPORT_FILE_NAME ${export_header}
  )
install(
  FILES ${export_header}
  DESTINATION include COMPONENT Development
  )

# Install library
install(
  TARGETS IntersonArrayCxx
  EXPORT IntersonArraySDKCxxTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT Runtime
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT Development
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT Development
  )

# Copy "IntersonArray.dll" and "IntersonArrayCxx.dll" to allow tests and applications to be executed from the build directory
if( BUILD_TESTING )
  add_custom_command( TARGET IntersonArrayCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/bin/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:IntersonArrayCxx>" "${PROJECT_BINARY_DIR}/bin/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:IntersonArrayCxx>" "${PROJECT_BINARY_DIR}/bin/"
    )
endif()
if( BUILD_APPLICATIONS )
  add_custom_command( TARGET IntersonArrayCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/app/bin/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/app/bin/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:IntersonArrayCxx>" "${PROJECT_BINARY_DIR}/app/bin/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonArraySDK_DIR}/Libraries/IntersonArray.dll" "${PROJECT_BINARY_DIR}/app/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:IntersonArrayCxx>" "${PROJECT_BINARY_DIR}/app/bin"
    )
endif()
