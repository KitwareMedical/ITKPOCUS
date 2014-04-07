#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Windows.h> // Sleep


int main( int argc, char * argv[] )
{
  IntersonCxx::Controls::HWControls hwControls;
  IntersonCxx::Imaging::Scan2DClass scan2D;
  typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;

  typedef IntersonCxx::Controls::HWControls::FoundProbesType FoundProbesType;
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

  std::cout << "\nScan is on: " << scan2D.GetScanOn() << std::endl;
  std::cout << "RF Data transfer is on: " << scan2D.GetRFData() << std::endl;

  int ret = EXIT_SUCCESS;
  
  const int maxVectors = Scan2DClassType::MAX_VECTORS;
  const int maxSamples = Scan2DClassType::MAX_SAMPLES;
  const int maxPixels = maxVectors * maxSamples;

  scan2D.AbortScan();
  hwControls.StartMotor();
  std::cout << "\nStarting BMode scanning..." << std::endl;
  scan2D.StartReadScan();
  Sleep( 100 ); // "time to start"
  hwControls.StartBmode();
  std::cout << "Scan is on: " << scan2D.GetScanOn() << std::endl;

  hwControls.StopAcquisition();
  scan2D.StopReadScan();
  Sleep( 100 ); // "time to stop"
  scan2D.DisposeScan();
  hwControls.StopMotor();

  return ret;
}
