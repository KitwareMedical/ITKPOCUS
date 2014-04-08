find_package( SlicerExecutionModel REQUIRED )
include( ${SlicerExecutionModel_USE_FILE} )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

SEMMacroBuildCLI(
  NAME AcquireIntersonBMode
  TARGET_LIBRARIES ${ITK_LIBRARIES}
                   IntersonCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

if( BUILD_TESTING )
  set( TEMP ${PROJECT_BINARY_DIR}/Testing/Temporary )

  add_test( NAME AcquireIntersonBModeTest
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      ${TEMP}/AcquireIntersonBModeTest.mha
     )
  add_test( NAME AcquireIntersonBModeTest1
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      --frames 5
      ${TEMP}/AcquireIntersonBModeTest1.mha
     )
endif()
