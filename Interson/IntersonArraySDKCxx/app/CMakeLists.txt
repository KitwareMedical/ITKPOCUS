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
find_package( SlicerExecutionModel REQUIRED )
include( ${SlicerExecutionModel_USE_FILE} )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

SEMMacroBuildCLI(
  NAME AcquireIntersonArrayBMode
  TARGET_LIBRARIES ${ITK_LIBRARIES}
                   IntersonArrayCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

SEMMacroBuildCLI(
  NAME AcquireIntersonArrayRF
  TARGET_LIBRARIES ${ITK_LIBRARIES}
                   IntersonArrayCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

SEMMacroBuildCLI(
  NAME PrintIntersonArrayProbeInfo
  TARGET_LIBRARIES IntersonArrayCxx
  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
                      ${PROJECT_BINARY_DIR}/include
  )

if( BUILD_TESTING )
  set( TEMP ${PROJECT_BINARY_DIR}/Testing/Temporary )

  add_test( NAME AcquireIntersonArrayBModeTest
    COMMAND $<TARGET_FILE:AcquireIntersonArrayBMode>
      ${TEMP}/AcquireIntersonArrayBModeTest.mha
     )
  add_test( NAME AcquireIntersonArrayBModeTestFrames
    COMMAND $<TARGET_FILE:AcquireIntersonArrayBMode>
      --frames 5
      ${TEMP}/AcquireIntersonArrayBModeTestFrames.mha
     )
  add_test( NAME AcquireIntersonArrayBModeTestVoltage
    COMMAND $<TARGET_FILE:AcquireIntersonArrayBMode>
      --highVoltage 30
      ${TEMP}/AcquireIntersonArrayBModeTestVoltage.mha
     )
  add_test( NAME AcquireIntersonArrayBModeTestGain
    COMMAND $<TARGET_FILE:AcquireIntersonArrayBMode>
      --gain 50
      ${TEMP}/AcquireIntersonArrayBModeTestGain.mha
     )

  add_test( NAME AcquireIntersonArrayRFTest
    COMMAND $<TARGET_FILE:AcquireIntersonArrayRF>
      ${TEMP}/AcquireIntersonArrayRFTest.mha
     )
  add_test( NAME AcquireIntersonArrayRFTestFrames
    COMMAND $<TARGET_FILE:AcquireIntersonArrayRF>
      --frames 5
      ${TEMP}/AcquireIntersonArrayRFTestFrames.mha
     )
  add_test( NAME AcquireIntersonArrayRFTestVoltage
    COMMAND $<TARGET_FILE:AcquireIntersonArrayRF>
      --highVoltage 30
      ${TEMP}/AcquireIntersonArrayRFTestVoltage.mha
     )
  add_test( NAME PrintIntersonArrayProbeInfoTest
    COMMAND $<TARGET_FILE:PrintIntersonArrayProbeInfo>
     )
endif()
