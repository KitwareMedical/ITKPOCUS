#include <Windows.h> // Sleep

#include "itkImageFileWriter.h"

#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"

#include "AcquireIntersonBModeCLP.h"

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  const unsigned int Dimension = 3;
  typedef unsigned char                      PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef IntersonCxx::Controls::HWControls HWControlsType;
  IntersonCxx::Controls::HWControls hwControls;

  typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;
  Scan2DClassType scan2D;

  typedef HWControlsType::FoundProbesType FoundProbesType;

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

  return EXIT_SUCCESS;
}
