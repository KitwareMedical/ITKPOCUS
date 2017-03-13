/*=========================================================================

Library:   IntersonArray

Copyright Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 ( the "License" );
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#include <Windows.h> // Sleep

#include "itkImageFileWriter.h"

#include "IntersonArrayCxxControlsHWControls.h"
#include "IntersonArrayCxxImagingContainer.h"

#include "AcquireIntersonArrayBModeCLP.h"

typedef IntersonArrayCxx::Imaging::Container ContainerType;

const unsigned int Dimension = 3;
typedef ContainerType::PixelType           PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;


struct CallbackClientData
{
  ImageType *        Image;
  itk::SizeValueType FrameIndex;
};


void __stdcall pasteIntoImage( PixelType * buffer, void * clientData )
{
  CallbackClientData * callbackClientData =
    static_cast< CallbackClientData * >( clientData );
  ImageType * image = callbackClientData->Image;

  const ImageType::RegionType & largestRegion =
    image->GetLargestPossibleRegion();
  const ImageType::SizeType imageSize = largestRegion.GetSize();
  const itk::SizeValueType imageFrames = imageSize[2];
  if( callbackClientData->FrameIndex >= imageFrames )
    {
    return;
    }

  const int framePixels = imageSize[0] * imageSize[1];

  PixelType * imageBuffer = image->GetPixelContainer()->GetBufferPointer();
  imageBuffer += framePixels * callbackClientData->FrameIndex;
  std::memcpy( imageBuffer, buffer, framePixels * sizeof( PixelType ) );

  std::cout << "Acquired frame: " << callbackClientData->FrameIndex
    << std::endl;
  ++(callbackClientData->FrameIndex);
}


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;
  HWControlsType hwControls;

  const int steering = 0;
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

  HWControlsType::FrequenciesType frequencies;
  hwControls.GetFrequency( frequencies );
  if( !hwControls.SetFrequencyAndFocus( frequencyIndex, focusIndex,
      steering ) )
    {
    std::cerr << "Could not set the frequency." << std::endl;
    return EXIT_FAILURE;
    }

  if( !hwControls.SendHighVoltage( highVoltage, highVoltage ) )
    {
    std::cerr << "Could not set the high voltage." << std::endl;
    return EXIT_FAILURE;
    }
  if( !hwControls.EnableHighVoltage() )
    {
    std::cerr << "Could not enable high voltage." << std::endl;
    return EXIT_FAILURE;
    }

  if( !hwControls.SendDynamic( gain ) )
    {
    std::cerr << "Could not set dynamic gain." << std::endl;
    }

  hwControls.DisableHardButton();

  ContainerType container;
  container.AbortScan();
  container.SetRFData( false );

  const int height = hwControls.GetLinesPerArray();
  const int width = 512; //container.MAX_SAMPLES;
  if( hwControls.ValidDepth( depth ) == depth )
    {
    ContainerType::ScanConverterError converterErrorIdle =
      container.IdleInitScanConverter( depth, width, height, probeId,
        steering, false, false, 0 );
    ContainerType::ScanConverterError converterError =
      container.HardInitScanConverter( depth, width, height, steering );
    if( converterError != ContainerType::SUCCESS )
      {
      std::cerr << "Error during hard scan converter initialization: "
                << converterError << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    std::cerr << "Invalid requested depth for probe." << std::endl;
    }

  const int scanWidth = container.GetWidthScan();
  const int scanHeight = container.GetHeightScan();
  std::cout << "Width = " << width << std::endl;
  std::cout << "Height = " << height << std::endl;
  std::cout << "ScanWidth = " << scanWidth << std::endl;
  std::cout << "ScanHeight = " << scanHeight << std::endl;
  std::cout << "MM per Pixel = " << container.GetMmPerPixel() << std::endl;

  const itk::SizeValueType framesToCollect = frames;

  ImageType::Pointer image = ImageType::New();
  typedef ImageType::RegionType RegionType;
  RegionType imageRegion;
  ImageType::IndexType imageIndex;
  imageIndex.Fill( 0 );
  imageRegion.SetIndex( imageIndex );
  ImageType::SizeType imageSize;
  imageSize[0] = width;
  imageSize[1] = height;
  imageSize[2] = framesToCollect;
  imageRegion.SetSize( imageSize );
  image->SetRegions( imageRegion );
  image->Allocate();

  CallbackClientData clientData;
  clientData.Image = image.GetPointer();
  clientData.FrameIndex = 0;

  container.SetNewImageCallback( &pasteIntoImage, &clientData );
  container.StartReadScan();
  Sleep( 100 ); // "time to start"
  if( !hwControls.StartBmode() )
    {
    std::cerr << "Could not start B-mode collection." << std::endl;
    return EXIT_FAILURE;
    };

  int c = 0;
  while( clientData.FrameIndex < framesToCollect && c < 100 )
    {
    std::cout << clientData.FrameIndex << " of " << framesToCollect
      << std::endl;
    std::cout << c << " of 100" << std::endl;
    Sleep( 100 );
    ++c;
    }

  hwControls.StopAcquisition();
  container.StopReadScan();
  Sleep( 100 ); // "time to stop"
  container.DisposeScan();

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

  return EXIT_SUCCESS;
}
