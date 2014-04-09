#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"

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
  // TODO make CLI option
  std::cout << "\nFrequencies: [Hz]" << std::endl;
  for( size_t ii = 0; ii < frequencies.size(); ++ii )
    {
    std::cout << "    " << ii << ": " << frequencies[ii] << std::endl;
    }
  /*
  // TODO make CLI option
  hwControls.SendHighVoltage( 50 );
  hwControls.EnableHighVoltage();
  // TODO make CLI option
  hwControls.SendDynamic( 50 );

  scan2D.AbortScan();
  if( !hwControls.StartMotor() )
    {
    std::cerr << "Could not start motor." << std::endl;
    return EXIT_FAILURE;
    };
  scan2D.StartReadScan();
  Sleep( 100 ); // "time to start"
  if( !hwControls.StartBmode() )
    {
    std::cerr << "Could not start B-mode collection." << std::endl;
    return EXIT_FAILURE;
    };

  while( clientData.FrameIndex < framesToCollect )
    {
    Sleep( 100 );
    }

  hwControls.StopAcquisition();
  scan2D.StopReadScan();
  Sleep( 100 ); // "time to stop"
  scan2D.DisposeScan();
  hwControls.StopMotor();

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  writer->SetInput( image );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }

  */
  return EXIT_SUCCESS;
}
