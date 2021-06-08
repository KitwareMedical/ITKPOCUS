cmake_minimum_required(VERSION 3.18.1)

project(clarius-image-server VERSION "1.0")
set(CMAKE_CXX_STANDARD 11)

set(Project_Main_Dir ${CMAKE_CURRENT_LIST_DIR}/src/main)
set(Project_Src_Dir ${CMAKE_CURRENT_LIST_DIR}/src/main/cpp)

include(FetchContent)

if(ON_ANDROID)
  add_library(pthread STATIC IMPORTED)
  set_target_properties(pthread PROPERTIES IMPORTED_LOCATION ${Project_Src_Dir}/libpthread.a)
endif()

########################################
## OpenIGTLink
########################################

set(BUILD_TESTING OFF CACHE INTERNAL "Disable testing")
set(USE_GTEST OFF CACHE INTERNAL "Don't use gtest")

FetchContent_Declare(OpenIGTLink
  GIT_REPOSITORY "https://github.com/openigtlink/OpenIGTLink.git"
  GIT_SHALLOW ON)

FetchContent_MakeAvailable(OpenIGTLink)

########################################
## clarius-image-server
########################################


list(APPEND Project_Sources
  ${Project_Src_Dir}/server.cpp)

add_library(clarius-image-server SHARED)
# adds include dir (android studio needs this)
include_directories(${Project_Src_Dir})
target_sources(clarius-image-server PUBLIC ${Project_Sources})
target_link_libraries(clarius-image-server PUBLIC OpenIGTLink pthread)
target_compile_options(clarius-image-server PRIVATE -Wall -Wextra)
set_target_properties(clarius-image-server PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION}
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

########################################
## swig
########################################

set(Java_Gen_Pkg com.igtl)
string(REPLACE "." "/" Java_Gen_Subdir ${Java_Gen_Pkg})
set(Java_Gen_Dir ${Project_Main_Dir}/java/${Java_Gen_Subdir})

# pass -module option to swig
cmake_policy(SET CMP0086 NEW)

# generate standard names for swig targets
cmake_policy(SET CMP0078 NEW)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
if(ON_ANDROID)
  set(JAVA_INCLUDE_PATH2 NotNeeded)
endif()
set(JAVA_AWT_INCLUDE_PATH NotNeeded)

# needed for java swig
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

list(APPEND Swig_Sources
        ${Project_Src_Dir}/java.i)

set_source_files_properties(${Swig_Sources} PROPERTIES CPLUSPLUS ON)

list(APPEND CMAKE_SWIG_FLAGS -package ${Java_Gen_Pkg})

swig_add_library(
  igtljava
  TYPE MODULE
  LANGUAGE java
  OUTPUT_DIR ${Java_Gen_Dir}
  SOURCES ${Swig_Sources} ${Project_Sources})

# on Android, pthread links to the dummy libpthread.a
target_link_libraries(igtljava PRIVATE OpenIGTLink pthread)
# needed to avoid fatal error: server.h: No such file or directory "server.h"
target_include_directories(igtljava PUBLIC ${Project_Src_Dir})
set_target_properties(igtljava PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set_property(TARGET igtljava PROPERTY
  SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)

########################################
## Android logging
########################################

if(ON_ANDROID)
  find_library(log-lib log)
  target_link_libraries(clarius-image-server PRIVATE ${log-lib})
  target_link_libraries(igtljava PRIVATE ${log-lib})
endif()
