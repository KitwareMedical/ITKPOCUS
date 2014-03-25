#include "IntersonCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char * argv[] )
{
  IntersonCxx::Controls::HWControls hwControls;

  std::cout << "ID_GP_3_5MHz: " << hwControls.ID_GP_3_5MHz << std::endl;

  std::cout << "ProbeID: " << static_cast< int >( hwControls.GetProbeID() ) << std::endl;

  return EXIT_SUCCESS;
}

