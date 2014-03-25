#include "IntersonCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char * argv[] )
{
  IntersonCxx::Controls::HWControls hwControls;

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

  return EXIT_SUCCESS;
}

