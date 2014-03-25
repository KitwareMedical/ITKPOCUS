cmake_minimum_required( VERSION 2.8 )

project( IntersonSDKCxx )

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_BINARY_DIR}/include )

add_subdirectory( src )

include( CTest )
if( BUILD_TESTING )
  enable_testing()
  add_subdirectory( test )
endif()
