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

#include "IntersonArrayCxxImagingContainer.h"
#include "IntersonArrayCxxControlsHWControls.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"

#include "IntersonArrayServerRFCLP.h"

typedef IntersonArrayCxx::Imaging::Container ContainerType;
typedef ContainerType::RFImagePixelType    PixelType;
bool running;

BOOL WINAPI consoleHandler(DWORD signal) {

  if (signal == CTRL_C_EVENT)
  {
    running = false;
  }

  return TRUE;
}

struct CallbackClientData
{
  int FrameIndex;
  igtl::Socket * socket;
  igtl::ImageMessage * imgMsg;
  igtl::TimeStamp * ts;
};

void __stdcall pasteIntoImage( PixelType * buffer, void * clientData )
{
  CallbackClientData * callbackClientData =
    static_cast< CallbackClientData * >( clientData );  

  igtl::ImageMessage * message = callbackClientData->imgMsg;
  igtl::Socket * socket = callbackClientData->socket;
  igtl::TimeStamp * ts = callbackClientData->ts;
  
  const int framePixels = ContainerType::MAX_RFSAMPLES * ContainerType::NBOFLINES;    
  ++(callbackClientData->FrameIndex);  

  message->SetMessageID(callbackClientData->FrameIndex);
  ts->GetTime();
  igtlUint32 sec;
  igtlUint32 nsec;
  ts->GetTimeStamp(&sec, &nsec);
  message->SetTimeStamp(sec, nsec);

  std::memcpy(message->GetScalarPointer(), buffer, framePixels * sizeof(PixelType));  

  if (!socket)
  {
    return;
  }
  
  message->Pack();
  socket->Send(message->GetPackPointer(), message->GetPackSize());
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

  HWControlsType::FocusType focuses;
  hwControls.GetFocus(focuses);

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
  const double ns = ContainerType::MAX_RFSAMPLES; // number of samples
  const double fs = 30000; // [kHz]=[samples/ms] - sampling frequency
  const double depth = sos * ( ns - 1 ) / ( 2 * fs );
  const double depthCfm = depth/2;
  std::cout << "Depth: " << depth << "mm" << std::endl;
  std::cout << std::endl;

  container.SetRFData( true );
  container.IdleInitScanConverter( depth, width_samples, height_lines, probeId,
      steering, depthCfm, false, false, 0, false );
  container.HardInitScanConverter( depth, width_samples, height_lines, steering,
      depthCfm );  

  CallbackClientData clientData;
  clientData.FrameIndex = 0;

  // size parameters
  const short frameRate = hwControls.GetProbeFrameRate();
  int   size[] = { ContainerType::MAX_RFSAMPLES, ContainerType::NBOFLINES, 1 };       // image dimension
  float spacing[] = { sos / (2 * fs), 38.0 / (height_lines - 1), 1.0 / frameRate };     // spacing (mm/pixel)
  int   svsize[] = { ContainerType::MAX_RFSAMPLES, ContainerType::NBOFLINES, 1 };       // sub-volume size
  int   svoffset[] = { 0, 0, 0 };           // sub-volume offset
  int   scalarType = igtl::ImageMessage::TYPE_INT16;// scalar type

  //------------------------------------------------------------
  // Create a new IMAGE type message
  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacing);
  imgMsg->SetScalarType(scalarType);
  imgMsg->SetDeviceName("IntersonRF");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  IANA_ENCODING_TYPE codingScheme = IANA_TYPE_US_ASCII;

  //Add meta data here
  imgMsg->SetMetaDataElement("Frequency (MHz)", codingScheme, std::to_string(frequencies[frequencyIndex]));
  imgMsg->SetMetaDataElement("Focus (mm)", codingScheme, std::to_string(focuses[focusIndex]));
  imgMsg->SetMetaDataElement("High voltage", codingScheme, std::to_string(highVoltage));
  imgMsg->SetMetaDataElement("SOS", codingScheme, std::to_string(sos));
  imgMsg->SetMetaDataElement("FPS", codingScheme, std::to_string(frameRate));
  imgMsg->SetMetaDataElement("Steering", codingScheme, std::to_string(steering));
  imgMsg->SetMetaDataElement("Image Type", codingScheme, "RF");
  imgMsg->SetMetaDataElement("Timestamp seconds", codingScheme, "0");
  imgMsg->SetMetaDataElement("Timestamp nseconds", codingScheme, "0");

  imgMsg->AllocateScalars();
  imgMsg->SetEndian(igtl::ImageMessage::ENDIAN_LITTLE);
  igtl::Matrix4x4 matrix;
  matrix[0][0] = 0.0;  matrix[1][0] = -1.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 1.0;  matrix[1][1] = 0.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  imgMsg->SetMatrix(matrix);
  igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
  clientData.ts = ts.GetPointer();
  clientData.imgMsg = imgMsg.GetPointer();

  //socket setup
  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
  {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
  }
  std::cerr << "Created Server socket." << std::endl;


  igtl::Socket::Pointer socket;
  clientData.socket = socket;

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

  if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) 
  {
    std::cout << "Could not set control handler" << std::endl;
    return 1;
  }
  running = true;
  std::cout << "Server is running, use Ctrl-C to stop" << std::endl;
  while( running )
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    clientData.socket = socket.GetPointer();
      if (socket.IsNotNull()) // if client connected
      {        
        Sleep(10000);  //chill for a bit
      }
      
    }

  std::cout << "Shutting down" << std::endl;
  hwControls.StopAcquisition();
  container.StopReadScan();
  if (socket)
  {
    socket->CloseSocket();
  }
  
  Sleep( 100 ); // "time to stop"
  return ret;
}
