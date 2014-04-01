#include "IntersonCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char * argv[] )
{
  IntersonCxx::Controls::HWControls hwControls;
  typedef IntersonCxx::Controls::HWControls HWControlsType;

  std::cout << "ID_GP_3_5MHz: " << hwControls.ID_GP_3_5MHz << std::endl;

  std::cout << "ProbeID: " << static_cast< int >( hwControls.GetProbeID() ) << std::endl;

  typedef IntersonCxx::Controls::HWControls::FoundProbesType FoundProbesType;
  FoundProbesType foundProbes;
  hwControls.FindAllProbes( foundProbes );
  std::cout << "Found Probes: " << std::endl;
  for( size_t ii = 0; ii < foundProbes.size(); ++ii )
    {
    std::cout << "    " << foundProbes[ii] << std::endl;
    }
  if( foundProbes.size() == 0 )
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
  if( !hwControls.SetFrequency( frequencies[0] ) )
    {
    std::cerr << "Could not set the frequency." << std::endl;
    return EXIT_FAILURE;
    }

  hwControls.SendHighVoltage( 50 );
  hwControls.EnableHighVoltage();
  hwControls.DisableHighVoltage();

  hwControls.SendDynamic( 50 );
  
  hwControls.StartMotor();
  hwControls.StopMotor();

  hwControls.EnableHardButton();
  hwControls.DisableHardButton();
  const unsigned char button = hwControls.ReadHardButton();
  std::cout << "\nButton: " << static_cast< int >( button ) << std::endl;

  std::cout << "\nStarting acquisition..." << std::endl;
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

  const short frameRate = hwControls.GetProbeFrameRate( 40 );
  std::cout << "Frame rate: " << frameRate << std::endl;
  std::cout << "Serial number: " << hwControls.GetProbeSerialNumber() << std::endl;
  std::cout << "FPGA Version: " << hwControls.ReadFPGAVersion() << std::endl;

  return EXIT_SUCCESS;
}

