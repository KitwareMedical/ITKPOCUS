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

#include "igtlImageMessage.h"
#include "igtlServerSocket.h"

#include "IntersonArrayServerBModeCLP.h"

bool running;

typedef IntersonArrayCxx::Imaging::Container ContainerType;

const unsigned int Dimension = 3;
typedef ContainerType::PixelType           PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
    {
        running = false;
    }

    return TRUE;
}

struct CallbackClientData
{
    itk::SizeValueType FrameIndex;
    igtl::Socket* socket;
    igtl::ImageMessage* imgMsg;
    igtl::TimeStamp* ts;
    int width;
    int height;
};


void __stdcall pasteIntoImage(PixelType* buffer, void* clientData)
{
    CallbackClientData* callbackClientData =
        static_cast<CallbackClientData*>(clientData);

    std::cout << "Acquired frame: " << callbackClientData->FrameIndex
        << std::endl;
    ++(callbackClientData->FrameIndex);

    // sending to openigtlink
    igtl::ImageMessage* message = callbackClientData->imgMsg;
    igtl::Socket* socket = callbackClientData->socket;
    igtl::TimeStamp* ts = callbackClientData->ts;

    int framePixels = callbackClientData->width * callbackClientData->height;

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


int main(int argc, char* argv[])
{
    PARSE_ARGS;

    typedef IntersonArrayCxx::Controls::HWControls HWControlsType;
    HWControlsType hwControls;

    const int steering = 0;
    typedef HWControlsType::FoundProbesType FoundProbesType;
    FoundProbesType foundProbes;
    hwControls.FindAllProbes(foundProbes);
    if (foundProbes.empty())
    {
        std::cerr << "Could not find the probe." << std::endl;
        return EXIT_FAILURE;
    }
    hwControls.FindMyProbe(0);
    const unsigned int probeId = hwControls.GetProbeID();
    if (probeId == 0)
    {
        std::cerr << "Could not find the probe." << std::endl;
        return EXIT_FAILURE;
    }

    HWControlsType::FrequenciesType frequencies;
    hwControls.GetFrequency(frequencies);

    HWControlsType::FocusType focuses;
    hwControls.GetFocus(focuses);

    if (!hwControls.SetFrequencyAndFocus(frequencyIndex, focusIndex,
        steering))
    {
        std::cerr << "Could not set the frequency." << std::endl;
        return EXIT_FAILURE;
    }

    if (!hwControls.SendHighVoltage(highVoltage, highVoltage))
    {
        std::cerr << "Could not set the high voltage." << std::endl;
        return EXIT_FAILURE;
    }
    if (!hwControls.EnableHighVoltage())
    {
        std::cerr << "Could not enable high voltage." << std::endl;
        return EXIT_FAILURE;
    }

    if (!hwControls.SendDynamic(gain))
    {
        std::cerr << "Could not set dynamic gain." << std::endl;
    }

    hwControls.DisableHardButton();

    ContainerType container;
    container.SetRFData(false);

    const int height = hwControls.GetLinesPerArray();
    const int width = container.MAX_SAMPLES;
    const int depth = 100;
    const int depthCfm = 50;
    if (hwControls.ValidDepth(depth) == depth)
    {
        ContainerType::ScanConverterError converterErrorIdle =
            container.IdleInitScanConverter(depth, width, height, probeId,
                steering, depthCfm, false, false, 0, false);
        ContainerType::ScanConverterError converterError =
            container.HardInitScanConverter(depth, width, height, steering,
                depthCfm);
        if (converterError != ContainerType::SUCCESS)
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
    std::cout << "Depth: " << depth << "mm" << std::endl;
    std::cout << std::endl;

    int imageSize[3];
    imageSize[0] = width;
    imageSize[1] = height;
    imageSize[2] = 1;
    
    float imageSpacing[3];
    imageSpacing[0] = container.GetMmPerPixel() / 10.;
    imageSpacing[1] = 38.0 / (height - 1);
    const short frameRate = hwControls.GetProbeFrameRate();
    imageSpacing[2] = 1.0 / frameRate;
    
    //------------------------------------------------------------
      // Create a new IMAGE type message
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    imgMsg->SetDimensions(imageSize);
    imgMsg->SetSpacing(imageSpacing);
    imgMsg->SetScalarType(igtl::ImageMessage::TYPE_UINT8);
    imgMsg->SetDeviceName("IntersonBMode");

    int   svsize[] = { imageSize[0], imageSize[1], 1 };       // sub-volume size
    int   svoffset[] = { 0, 0, 0 };

    imgMsg->SetSubVolume(svsize, svoffset);
    imgMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    IANA_ENCODING_TYPE codingScheme = IANA_TYPE_US_ASCII;

    //Add meta data here
    imgMsg->SetMetaDataElement("Frequency (MHz)", codingScheme, std::to_string(frequencies[frequencyIndex]));
    imgMsg->SetMetaDataElement("Focus (mm)", codingScheme, std::to_string(focuses[focusIndex]));
    imgMsg->SetMetaDataElement("High voltage", codingScheme, std::to_string(highVoltage));
    imgMsg->SetMetaDataElement("FPS", codingScheme, std::to_string(frameRate));
    imgMsg->SetMetaDataElement("Steering", codingScheme, std::to_string(steering));
    imgMsg->SetMetaDataElement("Image Type", codingScheme, "RF");
    imgMsg->SetMetaDataElement("Timestamp seconds", codingScheme, "0");
    imgMsg->SetMetaDataElement("Timestamp nseconds", codingScheme, "0");

    imgMsg->AllocateScalars();
    imgMsg->SetEndian(igtl::ImageMessage::ENDIAN_LITTLE);
    imgMsg->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_LPS); // LPS
    igtl::Matrix4x4 matrix;
    matrix[0][0] = 0.0;  matrix[1][0] = -1.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
    matrix[0][1] = 1.0;  matrix[1][1] = 0.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
    matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
    matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
    /*igtl::Matrix4x4 matrix;
    matrix[0][0] = 0.0;  matrix[0][1] = 1.0;  matrix[0][2] = 0.0; matrix[0][3] = 0.0;
    matrix[1][2] = -1.0;  matrix[1][2] = 0.0;  matrix[1][2] = 0.0; matrix[1][3] = 0.0;
    matrix[2][2] = 0.0;  matrix[2][2] = 0.0;  matrix[2][2] = 1.0; matrix[2][3] = 0.0;
    matrix[3][2] = 0.0;  matrix[3][2] = 0.0;  matrix[3][2] = 0.0; matrix[3][3] = 1.0;*/


//    matrix[0][0] = -1.0;  matrix[0][1] = 0.0;  matrix[0][2] = 0.0; matrix[0][3] = 0.0;
//    matrix[1][2] = 0.0;  matrix[1][2] = 1.0;  matrix[1][2] = 0.0; matrix[1][3] = 0.0;
//    matrix[2][2] = 0.0;  matrix[2][2] = 0.0;  matrix[2][2] = 1.0; matrix[2][3] = 0.0;
//    matrix[3][2] = 0.0;  matrix[3][2] = 0.0;  matrix[3][2] = 0.0; matrix[3][3] = 1.0;
    imgMsg->SetMatrix(matrix);
    igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();

    CallbackClientData clientData;
    clientData.ts = ts.GetPointer();
    clientData.imgMsg = imgMsg.GetPointer();
    clientData.FrameIndex = 0;
    clientData.width = imageSize[0];
    clientData.height = imageSize[1];

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

    container.SetNewImageCallback(&pasteIntoImage, &clientData);
    container.StartReadScan();
    Sleep(100); // "time to start"
    if (!hwControls.StartBmode())
    {
        std::cerr << "Could not start B-mode collection." << std::endl;
        return EXIT_FAILURE;
    };

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        std::cout << "Could not set control handler" << std::endl;
        return 1;
    }
    running = true;
    std::cout << "Server is running, use Ctrl-C to stop" << std::endl;
    while (running)
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

    Sleep(100); // "time to stop"
    return EXIT_SUCCESS;
}
