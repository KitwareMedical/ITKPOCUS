#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char * argv[] )
{
  IntersonCxx::Controls::HWControls hwControls;
  IntersonCxx::Imaging::Scan2DClass scan2D;
  typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;

  std::cout << "Scan is on: " << scan2D.GetScanOn() << std::endl;
  std::cout << "RF Data transfer is on: " << scan2D.GetRFData() << std::endl;

  int ret = EXIT_SUCCESS;

  hwControls.StartMotor();
  unsigned char * bModeBuffer = new unsigned char[Scan2DClassType::MAX_VECTORS * Scan2DClassType::MAX_SAMPLES];
  scan2D.StartReadScan( bModeBuffer );

  hwControls.StopAcquisition();
  scan2D.StopReadScan();

  hwControls.StopMotor();

  delete[] bModeBuffer;

  return ret;
}
