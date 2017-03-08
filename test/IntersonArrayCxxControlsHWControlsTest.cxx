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

int main( int argc, char * argv[] )
{
  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;

  HWControlsType hwControls;


  std::cout << "ID_CA_OP_10MHz: " << hwControls.ID_CA_OP_10MHz << std::endl;

  std::cout << "ProbeID: " << static_cast< int >(
    hwControls.GetProbeID() ) << std::endl;

  typedef IntersonArrayCxx::Controls::HWControls::FoundProbesType
    FoundProbesType;
  FoundProbesType foundProbes;
  hwControls.FindAllProbes( foundProbes );
  std::cout << "Found Probes: " << std::endl;
  for( size_t ii = 0; ii < foundProbes.size(); ++ii )
    {
    std::cout << "    " << foundProbes[ii] << std::endl;
    }
  if( foundProbes.empty() )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  hwControls.FindMyProbe( 0 );
  const short probeId = hwControls.GetProbeID();
  std::cout << "ProbeID after FindMyProbe: " << probeId << std::endl;
  if( probeId == 0 )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  int depth = hwControls.ValidDepth( 3 );
  std::cout << "\nValidDepth( 3 ): " << depth << std::endl;
  depth = hwControls.ValidDepth( 1000 );
  std::cout << "ValidDepth( 1000 ): " << depth << std::endl;

  HWControlsType::FrequenciesType frequencies;
  hwControls.GetFrequency( frequencies );
  std::cout << "\nFrequencies: " << std::endl;
  for( size_t ii = 0; ii < frequencies.size(); ++ii ) 
    {
    std::cout << "    " << frequencies[ii] << std::endl;
    }
  HWControlsType::FocusType focus;
  hwControls.GetFocus( focus );
  std::cout << "\nFocus: " << std::endl;
  for( size_t ii = 0; ii < focus.size(); ++ii ) 
    {
    std::cout << "    " << focus[ii] << std::endl;
    }
  if( !hwControls.SetFrequencyAndFocus( 0, 0, 0 ) )
    {
    std::cerr << "Could not set the frequency." << std::endl;
    return EXIT_FAILURE;
    }

  hwControls.SendHighVoltage( 50, 50 );
  hwControls.EnableHighVoltage();
  hwControls.DisableHighVoltage();

  hwControls.SendDynamic( 50 );
  
  hwControls.EnableHardButton();
  hwControls.DisableHardButton();
  const unsigned char button = hwControls.ReadHardButton();
  std::cout << "\nButton: " << static_cast< int >( button ) << std::endl;

  std::cout << "\nStarting acquisition...\n" << std::endl;
  if( !hwControls.StartBmode() )
    {
    std::cerr << "Could not start B mode acquision." << std::endl;
    return EXIT_FAILURE;
    }
  if( !hwControls.StopAcquisition() )
    {
    std::cerr << "Could not stop acquision." << std::endl;
    return EXIT_FAILURE;
    }
  if( !hwControls.StartRFmode() )
    {
    std::cerr << "Could not start RF acquision." << std::endl;
    return EXIT_FAILURE;
    }
  if( !hwControls.StopAcquisition() )
    {
    std::cerr << "Could not stop acquision." << std::endl;
    return EXIT_FAILURE;
    }

  const short frameRate = hwControls.GetProbeFrameRate();
  std::cout << "Frame rate: " << frameRate << std::endl;
  std::cout << "Serial number: " << hwControls.GetProbeSerialNumber()
    << std::endl;
  std::cout << "FPGA Version: " << hwControls.ReadFPGAVersion()
    << std::endl;
  std::cout << "OEM ID: " << hwControls.GetOEMId() << std::endl;
  std::cout << "Filter ID: " << hwControls.GetFilterId() << std::endl;

  return EXIT_SUCCESS;
}
