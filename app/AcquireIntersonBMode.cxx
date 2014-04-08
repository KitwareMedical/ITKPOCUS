#include <Windows.h> // Sleep

#include "itkImageFileWriter.h"

#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"

#include "AcquireIntersonBModeCLP.h"

const unsigned int Dimension = 3;
typedef unsigned char                      PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

struct CallbackClientData
{
  ImageType *        Image;
  itk::SizeValueType FrameIndex;
};

void __stdcall pasteIntoImage( PixelType * buffer, void * clientData )
{
  CallbackClientData * callbackClientData = static_cast< CallbackClientData * >( clientData );
  ++(callbackClientData->FrameIndex);
  std::cout << "FrameIndex: " << callbackClientData->FrameIndex << std::endl;
}

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
  hwControls.FindMyProbe( 0 );
  const unsigned int probeId = hwControls.GetProbeID();
  if( probeId == 0 )
    {
    std::cerr << "Could not find the probe." << std::endl;
    return EXIT_FAILURE;
    }

  int ret = EXIT_SUCCESS;

  CallbackClientData clientData;
  clientData.Image = NULL;
  clientData.FrameIndex = 0;

  scan2D.SetNewBmodeImageCallback( &pasteIntoImage, &clientData );

  scan2D.AbortScan();
  hwControls.StartMotor();
  scan2D.StartReadScan();
  Sleep( 100 ); // "time to start"
  hwControls.StartBmode();

  Sleep( 300 ); // "time to start"

  hwControls.StopAcquisition();
  scan2D.StopReadScan();
  Sleep( 100 ); // "time to stop"
  scan2D.DisposeScan();
  hwControls.StopMotor();

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  try
    {
    // writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }

  return ret;
}
