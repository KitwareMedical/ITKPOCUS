#[=======================================================================[.rst:
FindSonivate
-------

Finds the Sonivate library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Sonivate``
  The Sonivate::Sonivate library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Sonivate_FOUND``
  True if the system has the Sonivate library.
``Sonivate_INCLUDE_DIR``
  The directory containing ``ExternalInterface.h``.
``Sonivate_LIBRARY``
  The path to the Sonivate library.

#]=======================================================================]

if (NOT WIN32)
  message(FATAL_ERROR "Sonivate is only a windows library")
endif()

find_path(Sonivate_INCLUDE_DIR ExternalInterface.h)

find_library(Sonivate_LIBRARY_DEBUG
  NAMES UltrasoundInterfaceLib
  PATHS
    # If include dir points to Demo
    ${Sonivate_INCLUDE_DIR}/x64/Debug
    ${Sonivate_INCLUDE_DIR}/MCE/DEBUG
    # If include dir points to Demo/MCE
    ${Sonivate_INCLUDE_DIR}/DEBUG
)
find_library(Sonivate_LIBRARY_RELEASE
  NAMES UltrasoundInterfaceLib
  PATHS
    # If include dir points to Demo
    ${Sonivate_INCLUDE_DIR}/x64/Release
    ${Sonivate_INCLUDE_DIR}/MCE/RELEASE
    # If include dir points to Demo/MCE
    ${Sonivate_INCLUDE_DIR}/RELEASE
)

find_file(Sonivate_OpenCV_DLL
  NAMES opencv_world343.dll
  PATHS
    # If include dir points to Demo
    ${Sonivate_INCLUDE_DIR}
    # If include dir points to Demo/MCE
    ${Sonivate_INCLUDE_DIR}/..
)

file(GLOB Sonivate_RESOURCES_DEBUG ${Sonivate_INCLUDE_DIR}/*)
list(REMOVE_ITEM Sonivate_RESOURCES_DEBUG ${Sonivate_INCLUDE_DIR}/DEBUG)
list(REMOVE_ITEM Sonivate_RESOURCES_DEBUG ${Sonivate_INCLUDE_DIR}/RELEASE)
list(APPEND Sonivate_RESOURCES_DEBUG 
  ${Sonivate_INCLUDE_DIR}/DEBUG/UltrasoundInterfaceLib.lib)
list(APPEND Sonivate_RESOURCES_DEBUG 
  ${Sonivate_INCLUDE_DIR}/DEBUG/UltrasoundInterfaceLib.dll)
  
file(GLOB Sonivate_RESOURCES_RELEASE ${Sonivate_INCLUDE_DIR}/*)
list(REMOVE_ITEM Sonivate_RESOURCES_RELEASE ${Sonivate_INCLUDE_DIR}/DEBUG)
list(REMOVE_ITEM Sonivate_RESOURCES_RELEASE ${Sonivate_INCLUDE_DIR}/RELEASE)
list(APPEND Sonivate_RESOURCES_RELEASE 
  ${Sonivate_INCLUDE_DIR}/RELEASE/UltrasoundInterfaceLib.lib)
list(APPEND Sonivate_RESOURCES_RELEASE
  ${Sonivate_INCLUDE_DIR}/RELEASE/UltrasoundInterfaceLib.dll)  

include(SelectLibraryConfigurations)
select_library_configurations(Sonivate)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sonivate
  FOUND_VAR Sonivate_FOUND
  REQUIRED_VARS
    Sonivate_LIBRARY
    Sonivate_INCLUDE_DIR
    Sonivate_OpenCV_DLL
)

if(Sonivate_FOUND AND NOT TARGET Sonivate::Sonivate)
  add_library(Sonivate::Sonivate SHARED IMPORTED)
  
  if (Sonivate_LIBRARY_RELEASE)
    set_property(TARGET Sonivate::Sonivate APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(Sonivate::Sonivate PROPERTIES
      IMPORTED_LOCATION_RELEASE "${Sonivate_LIBRARY_RELEASE}"
    )
  endif()
  if (Sonivate_LIBRARY_DEBUG)
    set_property(TARGET Sonivate::Sonivate APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG
    )
    set_target_properties(Sonivate::Sonivate PROPERTIES
      IMPORTED_LOCATION_DEBUG "${Sonivate_LIBRARY_DEBUG}"
    )
  endif()

  set_target_properties(Sonivate::Sonivate PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Sonivate_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Sonivate_LIBRARY_DEBUG
  Sonivate_LIBRARY_RELEASE
  Sonivate_OpenCV_DLL
)