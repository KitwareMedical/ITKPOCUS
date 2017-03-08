/*=========================================================================

Library:   IntersonArrayArray

Copyright Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 ( the "License" );
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#include "IntersonArrayCxxImagingCapture.h"
#include "IntersonArrayCxxControlsHWControls.h"
#include "IntersonArrayCxxImagingScanConverter.h"
#include "IntersonArrayCxxImagingImageBuilding.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Windows.h> // Sleep

int main( int argc, char * argv[] )
{
  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;
  HWControlsType hwControls;

  typedef IntersonArrayCxx::Imaging::Capture CaptureType;
  CaptureType capture;

  typedef IntersonArrayCxx::Imaging::ImageBuilding ImageBuildingType;
  ImageBuildingType imageBuilding;

  typedef IntersonArrayCxx::Imaging::ScanConverter ScanConverterType;
  ScanConverterType scanConverter;

  typedef HWControlsType::FoundProbesType FoundProbesType;
  FoundProbesType foundProbes;
  hwControls.FindAllProbes( foundProbes );
  if( foundProbes.empty() )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  hwControls.FindMyProbe( 0 );
  const unsigned int probeId = hwControls.GetProbeID();
  std::cout << "ProbeID after FindMyProbe: " << probeId << std::endl;
  if( probeId == 0 )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  const int depth = 50;
  if( hwControls.ValidDepth( depth ) != depth )
    {
    std::cerr << "Did not request a valid depth" << std::endl;
    return EXIT_SUCCESS;
    }
  const int width = 776;
  const int height = 512; 

  int steering = 0;
  ScanConverterType::ScanConverterError converterError =
    scanConverter.IdleInitScanConverter( depth, width, height, probeId,
    steering, false, false, 0, imageBuilding );
  if( converterError != ScanConverterType::SUCCESS )
    {
    std::cerr << "Error during idle scan converter initialization: "
              << converterError << std::endl;
    return EXIT_FAILURE;
    }
  converterError = scanConverter.HardInitScanConverter( depth, width,
    height, steering, capture, imageBuilding );
  if( converterError != ScanConverterType::SUCCESS )
    {
    std::cerr << "Error during hard scan converter initialization: "
              << converterError << std::endl;
    return EXIT_FAILURE;
    }

  capture.AbortScan();
  std::cout << "\nStarting BMode scanning..." << std::endl;
  capture.StartReadScan();
  Sleep( 100 ); // "time to start"
  hwControls.StartBmode();

  std::cout << "\nScan is on: " << capture.GetScanOn() << std::endl;
  std::cout << "\nTrueDepth: " << scanConverter.GetTrueDepth() << std::endl;
  std::cout << "\nHeightScan: " << scanConverter.GetHeightScan() << std::endl;
  std::cout << "WidthScan:  " << scanConverter.GetWidthScan() << std::endl;
  std::cout << "\nMmPerPixel: " << scanConverter.GetMmPerPixel() << std::endl;
  std::cout << "\nZeroOfYScale: " << scanConverter.GetZeroOfYScale() << std::endl;

  hwControls.StopAcquisition();
  capture.StopReadScan();
  Sleep( 100 ); // "time to stop"
  capture.DisposeScan();

  return EXIT_SUCCESS;
}
