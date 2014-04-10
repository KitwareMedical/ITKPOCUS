#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"
#include "IntersonCxxImagingScanConverter.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Windows.h> // Sleep

int main( int argc, char * argv[] )
{
  typedef IntersonCxx::Controls::HWControls HWControlsType;
  HWControlsType hwControls;

  typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;
  Scan2DClassType scan2D;

  typedef IntersonCxx::Imaging::ScanConverter ScanConverterType;
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
  const bool upDown = true;
  const bool leftRight = true;
  const int width = 776;
  const int height = 512; 

  ScanConverterType::ScanConverterError converterError =
    scanConverter.HardInitScanConverter( depth,
                                         upDown,
                                         leftRight,
                                         width,
                                         height );
  if( converterError != ScanConverterType::SUCCESS )
    {
    std::cerr << "Error during scan converter initialization: "
              << converterError << std::endl;
    return EXIT_FAILURE;
    }

  scan2D.AbortScan();
  hwControls.StartMotor();
  std::cout << "\nStarting BMode scanning..." << std::endl;
  scan2D.StartReadScan();
  Sleep( 100 ); // "time to start"
  hwControls.StartBmode();

  std::cout << "\nScan is on: " << scan2D.GetScanOn() << std::endl;
  std::cout << "\nTrueDepth: " << scan2D.GetTrueDepth() << std::endl;
  std::cout << "\nHeightScan: " << scanConverter.GetHeightScan() << std::endl;
  std::cout << "WidthScan: " << scanConverter.GetWidthScan() << std::endl;
  std::cout << "\nMmPerPixel: " << scanConverter.GetMmPerPixel() << std::endl;
  std::cout << "\nZeroOfYScale: " << scanConverter.GetZeroOfYScale() << std::endl;

  hwControls.StopAcquisition();
  scan2D.StopReadScan();
  Sleep( 100 ); // "time to stop"
  scan2D.DisposeScan();
  hwControls.StopMotor();

  return EXIT_SUCCESS;
}
