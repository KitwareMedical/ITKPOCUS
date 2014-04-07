find_package( SlicerExecutionModel REQUIRED )
include( ${SlicerExecutionModel_USE_FILE} )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

SEMMacroBuildCLI(
  NAME AcquireIntersonBMode
  TARGET_LIBRARIES ${ITK_LIBRARIES}
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

if( BUILD_TESTING )
  add_test( NAME AcquireIntersonBModeTest
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      AcquireIntersonBModeTest.mha
   )
endif()
