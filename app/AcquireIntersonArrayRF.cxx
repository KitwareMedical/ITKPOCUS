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

#include "IntersonArrayCxxImagingContainer.h"
#include "IntersonArrayCxxControlsHWControls.h"

#include "AcquireIntersonArrayRFCLP.h"

typedef IntersonArrayCxx::Imaging::Container ContainerType;

const unsigned int Dimension = 3;
typedef ContainerType::RFImagePixelType    PixelType;
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
  ImageType * image =
    callbackClientData->Image;

  const ImageType::RegionType & largestRegion =
    image->GetLargestPossibleRegion();
  const ImageType::SizeType imageSize = largestRegion.GetSize();
  const itk::SizeValueType imageFrames = largestRegion.GetSize()[2];
  if( callbackClientData->FrameIndex >= imageFrames )
    {
    return;
    }

  const int framePixels = imageSize[0] * imageSize[1];
  std::cout << "frame pixels RF = " << framePixels << std::endl;

  PixelType * imageBuffer = image->GetPixelContainer()->GetBufferPointer();
  imageBuffer += framePixels * callbackClientData->FrameIndex;
  std::memcpy( imageBuffer, buffer, framePixels * sizeof( PixelType ) );

  std::cout << "Acquired frame RF: " << callbackClientData->FrameIndex
    << std::endl;
  ++(callbackClientData->FrameIndex);
}

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef IntersonArrayCxx::Controls::HWControls HWControlsType;
  IntersonArrayCxx::Controls::HWControls hwControls;

  int ret = EXIT_SUCCESS;

  int steering = 0;
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
    std::cerr << "Could not set the frequency and focus." << std::endl;
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

  hwControls.DisableHardButton();

  ContainerType container;
  container.SetHWControls(&hwControls);

  const int height_lines = hwControls.GetLinesPerArray();
  std::cout << "Lines per array: " << height_lines << std::endl;
  const int width_samples = ContainerType::MAX_RFSAMPLES;
  std::cout << "Max RF samples: " << width_samples << std::endl;
  const itk::SizeValueType framesToCollect = frames;

  container.AbortScan();
  container.SetRFData( true );

  container.IdleInitScanConverter( depth, width_samples, height_lines, probeId,
      steering, false, false, 0 );
  container.HardInitScanConverter( depth, width_samples, height_lines, steering );

  ImageType::Pointer image = ImageType::New();
  typedef ImageType::RegionType RegionType;
  RegionType imageRegion;
  ImageType::IndexType imageIndex;
  imageIndex.Fill( 0 );
  imageRegion.SetIndex( imageIndex );
  ImageType::SizeType imageSize;
  imageSize[0] = width_samples;
  imageSize[1] = height_lines;
  imageSize[2] = framesToCollect;
  imageRegion.SetSize( imageSize );
  image->SetRegions( imageRegion );
  image->Allocate();

  CallbackClientData clientData;
  clientData.Image = image.GetPointer();
  clientData.FrameIndex = 0;

  container.SetNewRFImageCallback( &pasteIntoImage, &clientData );

  std::cout << "StartRFReadScan" << std::endl;
  container.StartRFReadScan();
  Sleep( 100 ); // "time to start"
  std::cout << "StartRFmode" << std::endl;
  if( !hwControls.StartRFmode() )
    {
    std::cerr << "Could not start RF collection." << std::endl;
    return EXIT_FAILURE;
    }

  int c = 0;
  while( clientData.FrameIndex < framesToCollect && c < 10 )
    {
    std::cout << clientData.FrameIndex << " of " << framesToCollect
      << std::endl;
    std::cout << c << " of 10" << std::endl;
    Sleep( 100 );
    ++c;
    }

  std::cout << "StopAcquisition" << std::endl;
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

  return ret;
}
