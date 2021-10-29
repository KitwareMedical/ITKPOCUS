/*=========================================================================

Library:   IntersonArray

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
#include "IntersonArrayCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>

#include "PrintIntersonArrayProbeInfoCLP.h"

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  std::cout << "Start---" << std::endl;
  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;

  IntersonArrayCxx::Controls::HWControls hwControls;
 
  std::cout << "ProbeID: " << static_cast< int >(
    hwControls.GetProbeID() ) << std::endl;

  typedef HWControlsType::FoundProbesType FoundProbesType;
  FoundProbesType foundProbes;
  hwControls.FindAllProbes( foundProbes );
  if( foundProbes.empty() )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Probes Found: " << std::endl;
  for( size_t ii = 0; ii < foundProbes.size(); ++ii )
    {
    std::cout << "    " << ii << ": " << foundProbes[ii] << std::endl;
    }
  if( probeIndex < 0 || probeIndex >= foundProbes.size() )
    {
    std::cerr << "Invalid probe index." << std::endl;
    return EXIT_FAILURE;
    }
  hwControls.FindMyProbe( probeIndex );
  const unsigned int probeId = hwControls.GetProbeID();
  if( probeId == 0 )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  HWControlsType::FrequenciesType frequencies;
  hwControls.GetFrequency( frequencies );
  std::cout << "\nFrequencies: [Hz]" << std::endl;
  for( size_t ii = 0; ii < frequencies.size(); ++ii )
    {
    std::cout << "    " << ii << ": " << frequencies[ii] << std::endl;
    }

  HWControlsType::FocusType focus;
  hwControls.GetFocus( focus );
  std::cout << "\nFocus: [mm]" << std::endl;
  for( size_t ii = 0; ii < focus.size(); ++ii )
    {
    std::cout << "    " << ii << ": " << focus[ii] << std::endl;
    }

  std::cout << "\nSupported Depth [mm]: FrameRate [fps]" << std::endl;
  int depth = 20;
  while( hwControls.ValidDepth( depth ) == depth )
    {
    short frameRate = hwControls.GetProbeFrameRate();
    std::cout << "    " << depth << ":\t" << frameRate << std::endl;
    depth += 20;
    }

  std::cout << "Lines per array = " << hwControls.GetLinesPerArray()
    << std::endl;
  std::cout << "Array CompoundAngle = " << hwControls.GetCompoundAngle()
    << std::endl;
  std::cout << "Array Radius = " << hwControls.GetArrayRadius()
    << std::endl;
  std::cout << "Array Angle = " << hwControls.GetArrayAngle()
    << std::endl;
  std::cout << "Array Width = " << hwControls.GetArrayWidth()
    << std::endl;

  std::cout << "\nSerial number: " << hwControls.GetProbeSerialNumber()
    << std::endl;
  std::cout << "FPGA Version:  " << hwControls.ReadFPGAVersion()
    << std::endl;
  std::cout << "OEM ID:        " << hwControls.GetOEMId() << std::endl;
  std::cout << "Filter ID:     " << hwControls.GetFilterId() << std::endl;

  return EXIT_SUCCESS;
}
