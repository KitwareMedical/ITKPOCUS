cmake_minimum_required( VERSION 2.8 )

project( IntersonSDKCxx )

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_BINARY_DIR}/include )

find_path( IntersonSDK_DIR NAMES Libraries/Interson.dll
  PATHS C:/IntersonSDK )
if( NOT IntersonSDK_DIR )
  message( SEND_ERROR "Please specify the path to the IntersonSDK in
  IntersonSDK_DIR" )
endif()

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
