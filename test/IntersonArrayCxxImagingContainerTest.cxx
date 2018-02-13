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
#include "IntersonArrayCxxImagingContainer.h"
#include "IntersonArrayCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Windows.h> // Sleep

int main( int argc, char * argv[] )
{
  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;
  HWControlsType hwControls;

  typedef IntersonArrayCxx::Imaging::Container ContainerType;
  ContainerType container;

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
  int depthCfm = 50;
  ContainerType::ScanConverterError converterError =
    container.IdleInitScanConverter( depth, width, height, probeId,
    steering, depthCfm, false, false, 0, false );
  if( converterError != ContainerType::SUCCESS )
    {
    std::cerr << "Error during idle scan converter initialization: "
              << converterError << std::endl;
    return EXIT_FAILURE;
    }
  converterError = container.HardInitScanConverter( depth, width,
    height, steering, depthCfm );
  if( converterError != ContainerType::SUCCESS )
    {
    std::cerr << "Error during hard scan converter initialization: "
              << converterError << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "\nStarting BMode scanning..." << std::endl;
  container.StartReadScan();
  Sleep( 100 ); // "time to start"
  hwControls.StartBmode();

  std::cout << "\nScan is on: " << container.GetScanOn() << std::endl;
  std::cout << "\nTrueDepth: " << container.GetTrueDepth() << std::endl;
  std::cout << "\nHeightScan: " << container.GetHeightScan() << std::endl;
  std::cout << "WidthScan:  " << container.GetWidthScan() << std::endl;
  std::cout << "\nMmPerPixel: " << container.GetMmPerPixel() << std::endl;
  std::cout << "\nZeroOfYScale: " << container.GetZeroOfYScale() << std::endl;

  hwControls.StopAcquisition();
  container.StopReadScan();
  Sleep( 100 ); // "time to stop"

  return EXIT_SUCCESS;
}
