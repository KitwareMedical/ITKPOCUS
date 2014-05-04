#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"
#include "IntersonCxxIntersonClass.h"

#include "PrintIntersonProbeInfoCLP.h"

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef IntersonCxx::Controls::HWControls HWControlsType;
  IntersonCxx::Controls::HWControls hwControls;

  typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;
  Scan2DClassType scan2D;

  typedef HWControlsType::FoundProbesType FoundProbesType;
  FoundProbesType foundProbes;
  hwControls.FindAllProbes( foundProbes );
  if( foundProbes.empty() )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "\nProbes Found: " << std::endl;
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

  std::cout << "\nSupported Depth [mm]: FrameRate [fps]" << std::endl;
  int depth = 20;
  while( hwControls.ValidDepth( depth ) == depth )
    {
    short frameRate = hwControls.GetProbeFrameRate( depth );
    std::cout << "    " << depth << ":\t" << frameRate << std::endl;
    depth += 20;
    }

  std::cout << "\nSerial number: " << hwControls.GetProbeSerialNumber() << std::endl;
  std::cout << "FPGA Version:  " << hwControls.ReadFPGAVersion() << std::endl;
  std::cout << "OEM ID:        " << hwControls.GetOEMId() << std::endl;
  std::cout << "Filter ID:     " << hwControls.GetFilterId() << std::endl;

  IntersonCxx::IntersonClass intersonClass;
  std::cout << "SDK Version:   " << intersonClass.Version() << std::endl;

  return EXIT_SUCCESS;
}
