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

SEMMacroBuildCLI(
  NAME AcquireIntersonRF
  TARGET_LIBRARIES ${ITK_LIBRARIES}
                   IntersonCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

SEMMacroBuildCLI(
  NAME PrintIntersonProbeInfo
  TARGET_LIBRARIES IntersonCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

if( BUILD_TESTING )
  set( TEMP ${PROJECT_BINARY_DIR}/Testing/Temporary )

  add_test( NAME AcquireIntersonBModeTest
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      ${TEMP}/AcquireIntersonBModeTest.mha
     )
  add_test( NAME AcquireIntersonBModeTestFrames
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      --frames 5
      ${TEMP}/AcquireIntersonBModeTestFrames.mha
     )
  add_test( NAME AcquireIntersonBModeTestVoltage
    COMMAND $<TARGET_FILE:AcquireIntersonBMode>
      --highVoltage 30
      ${TEMP}/AcquireIntersonBModeTestTestVoltage.mha
     )

  add_test( NAME AcquireIntersonRFTest
    COMMAND $<TARGET_FILE:AcquireIntersonRF>
      ${TEMP}/AcquireIntersonRFTest.mha
     )
  add_test( NAME AcquireIntersonRFTestFrames
    COMMAND $<TARGET_FILE:AcquireIntersonRF>
      --frames 5
      ${TEMP}/AcquireIntersonRFTestFrames.mha
     )
  add_test( NAME AcquireIntersonRFTestVoltage
    COMMAND $<TARGET_FILE:AcquireIntersonRF>
      --highVoltage 30
      ${TEMP}/AcquireIntersonRFTestVoltage.mha
     )

  add_test( NAME PrintIntersonProbeInfoTest
    COMMAND $<TARGET_FILE:PrintIntersonProbeInfo>
     )
endif()
