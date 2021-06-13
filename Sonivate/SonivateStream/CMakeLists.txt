cmake_minimum_required(VERSION 3.18.0)
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 10.0.17134.0)
project(SonivateStream VERSION 0.0.1)

if(NOT WIN32)
  message(FATAL_ERROR "Only supported on Windows")
endif()

## SonivateIGT Project defines
list(APPEND CMAKE_MODULE_PATH ${SonivateStream_SOURCE_DIR}/CMake)

## IGTLink
find_package(OpenIGTLink REQUIRED)

## Sonivate
find_package(Sonivate REQUIRED)

FILE(GLOB SRC_FILES "./*.cpp")
FILE(GLOB HEAD_FILES "./*.h")
FILE(GLOB RC_FILES "./*.rc" "./res/*")
add_executable(SonivateStream ${SRC_FILES} ${HEAD_FILES} ${RC_FILES})

# TODO: confirm the /SAFESEH:NO flag is necessary
set_target_properties(SonivateStream PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
set(CMAKE_MFC_FLAG 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
include(MSVCLinkToStaticCRT)
msvc_link_to_static_crt()

#comment

target_link_libraries(SonivateStream
  ${OpenIGTLink_LIBRARIES}
  ${Sonivate_LIBRARIES}
)
target_include_directories(SonivateStream PUBLIC
  ${OpenIGTLink_INCLUDE_DIRS}
  ${Sonivate_INCLUDE_DIR}
)

FILE(GLOB DEMO_SRC_FILES "Demo_Test/*.cpp")
FILE(GLOB DEMO_HEAD_FILES "Demo_Test/*.h")
FILE(GLOB DEMO_RC_FILES "Demo_Test/*.rc" "Demo_Test/res/*")
add_executable(DemoTest ${DEMO_SRC_FILES} ${DEMO_HEAD_FILES} ${DEMO_RC_FILES})

# TODO: confirm the /SAFESEH:NO flag is necessary
set_target_properties(DemoTest PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
# add_definitions(-D_AFXDLL) # Necessary for stdafx.h
set(CMAKE_MFC_FLAG 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
include(MSVCLinkToStaticCRT)
msvc_link_to_static_crt()

target_link_libraries(DemoTest
  ${OpenIGTLink_LIBRARIES}
  ${Sonivate_LIBRARIES}
)
target_include_directories(DemoTest PUBLIC
  ${OpenIGTLink_INCLUDE_DIRS}
  ${Sonivate_INCLUDE_DIR}
)

#if(DEFINED ${Sonviate_RESOURCES_DEBUG})
  message("Copying resources debug")
	file(COPY ${Sonivate_RESOURCES_DEBUG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
#endif()

#if(DEFINED ${Sonivate_RESOURCES_RELEASE})
  message("Copying resources release")
	file(COPY ${Sonivate_RESOURCES_RELEASE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
#endif()
#CMAKE_CURRENT_BINARY_DIR=C:/src/TraumaticBrainInjury/Sonivate/SonivateStream-build
#message(STATUS "Sonivate DEBUG: ${Sonivate_RESOURCES_DEBUG}")
#message(STATUS "VS Build Directory: ${EXECUTABLE_OUTPUT_PATH}")

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
## Install
# Windows
#if (SonivateIGT_BUILD_MFC_WINDOWS_VERSION)
#  if("${CMAKE_MFC_FLAG}" STREQUAL "2")
#    set(CMAKE_INSTALL_MFC_LIBRARIES ON)
#    set(CMAKE_INSTALL_DEBUG_LIBRARIES ON)
#    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin)
#   include(InstallRequiredSystemLibraries)
#  endif()
#endif()

# Sonivate
#install(DIRECTORY $<TARGET_LINKER_FILE_DIR:Sonivate::Sonivate>/
#  DESTINATION bin
#)
#install(FILES ${Sonivate_OpenCV_DLL}
#  DESTINATION bin
#)

# SonivateIGT
#install(TARGETS SonivateIGTApp
#  RUNTIME
#  DESTINATION bin
#)
