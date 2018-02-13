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
#include "IntersonArrayCxxImagingContainer.h"
#include <iostream>

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include <msclr/auto_gcroot.h>

#using "IntersonArray.dll"

#pragma managed

namespace IntersonArrayCxx
{

namespace Imaging
{

//
// Begin Capture
//
ref class NewRFImageHandler
{
public:

  typedef Container::NewRFImageCallbackType   NewRFImageCallbackType;

  typedef IntersonArrayCxx::Imaging::Container::RFPixelType        RFPixelType;
  typedef IntersonArrayCxx::Imaging::Container::RFImagePixelType   RFImagePixelType;
  typedef cli::array< RFPixelType, 2 >                             RFArrayType;

  NewRFImageHandler( RFArrayType ^managedRFBuffer ):
    NewRFImageCallback( NULL ),
    NewRFImageCallbackClientData( NULL ),
    bufferWidth( Container::MAX_RFSAMPLES ),
    bufferHeight( Container::NBOFLINES ),
    NativeRFBuffer(new RFImagePixelType[Container::MAX_RFSAMPLES
      * Container::NBOFLINES] ),
    ManagedRFBuffer( managedRFBuffer )
  {
  }

  ~NewRFImageHandler()
  {
    delete [] NativeRFBuffer;
  }

  void HandleNewRFImage( IntersonArray::Imaging::Capture ^scan2D,
    System::EventArgs ^eventArgs )
  {
    if ( this->NewRFImageCallback != NULL )
    {
      for ( int ii = 0; ii < bufferHeight; ++ii )
      {
        for ( int jj = 0; jj < bufferWidth; ++jj )
        {
          this->NativeRFBuffer[bufferWidth * ii + jj] = (short)
            this->ManagedRFBuffer[ii, jj];
        }
      }
      this->NewRFImageCallback( this->NativeRFBuffer,
        this->NewRFImageCallbackClientData );
    }
  }

  void SetNewRFImageCallback( NewRFImageCallbackType callback,
    void *clientData )
  {
    this->NewRFImageCallback = callback;
    this->NewRFImageCallbackClientData = clientData;
  }

private:
  NewRFImageCallbackType   NewRFImageCallback;
  void                   * NewRFImageCallbackClientData;
  RFImagePixelType       * NativeRFBuffer;
  RFArrayType            ^ ManagedRFBuffer;
  int                      bufferWidth;
  int                      bufferHeight;
};


ref class NewImageHandler
{
public:
  typedef Container::NewImageCallbackType        NewImageCallbackType;

  typedef IntersonArrayCxx::Imaging::Container::PixelType     PixelType;
  typedef cli::array< byte, 2 >                               ArrayType;

  NewImageHandler( ArrayType ^managedBuffer ):
    NewImageCallback( NULL ),
    NewImageCallbackClientData( NULL ),
    bufferWidth( Container::MAX_SAMPLES ),
    bufferHeight( Container::NBOFLINES ),
    NativeBuffer( new PixelType[Container::MAX_SAMPLES *
      Container::NBOFLINES ] ),
    ManagedBuffer( managedBuffer )
  {
  }

  ~NewImageHandler()
  {
    delete [] NativeBuffer;
  }

  void HandleNewImage( IntersonArray::Imaging::Capture ^scan2D,
    System::EventArgs ^eventArgs )
  {
    if ( this->NewImageCallback != NULL )
    {
      for ( int ii = 0; ii < bufferHeight; ++ii )
      {
        for ( int jj = 0; jj < bufferWidth; ++jj )
        {
          this->NativeBuffer[bufferWidth * ii + jj] =
            this->ManagedBuffer[ii, jj];
        }
      }
      this->NewImageCallback( this->NativeBuffer,
        this->NewImageCallbackClientData );
    }
  }

  void SetNewImageCallback( NewImageCallbackType callback,
    void *clientData )
  {
    this->NewImageCallback = callback;
    this->NewImageCallbackClientData = clientData;
  }

private:
  NewImageCallbackType   NewImageCallback;
  void                 * NewImageCallbackClientData;
  PixelType            * NativeBuffer;
  ArrayType            ^ ManagedBuffer;
  int                    bufferWidth;
  int                    bufferHeight;
};

// End Capture

class ContainerImpl
{
public:
  typedef cli::array< Container::PixelType, 2 >      ArrayType;
  typedef cli::array< Container::RFPixelType, 2 >    RFArrayType;

  typedef Container::NewImageCallbackType      NewImageCallbackType;
  typedef Container::NewRFImageCallbackType    NewRFImageCallbackType;

  ContainerImpl()
  {
    WrappedScanConverter = gcnew IntersonArray::Imaging::ScanConverter();
    WrappedImageBuilding = gcnew IntersonArray::Imaging::ImageBuilding();
    WrappedCapture = gcnew IntersonArray::Imaging::Capture();

    Buffer = gcnew ArrayType( Container::NBOFLINES, Container::MAX_SAMPLES );
    Handler = gcnew NewImageHandler( Buffer );
    HandlerDelegate = gcnew
      IntersonArray::Imaging::Capture::NewImageHandler( Handler,
        & NewImageHandler::HandleNewImage );
    WrappedCapture->NewImageTick += HandlerDelegate;

    RFBuffer = gcnew RFArrayType( Container::NBOFLINES,
      Container::MAX_RFSAMPLES );
    RFHandler = gcnew NewRFImageHandler( RFBuffer );
    RFHandlerDelegate = gcnew
      IntersonArray::Imaging::Capture::NewImageHandler( RFHandler,
        & NewRFImageHandler::HandleNewRFImage );
  }

  ~ContainerImpl()
  {
    WrappedCapture->NewImageTick -= RFHandlerDelegate;
    WrappedCapture->NewImageTick -= HandlerDelegate;
  }

  bool GetCompound()
  {
    return WrappedScanConverter->Compound;
  }

  void SetCompound( bool value )
  {
    WrappedScanConverter->Compound = value;
  }

  bool GetDoubler()
  {
    return WrappedScanConverter->Doubler;
  }

  void SetDoubler( bool value )
  {
    WrappedScanConverter->Doubler = value;
  }

  int GetHeightScan() const
  {
    return WrappedScanConverter->HeightScan;
  }

  float GetMmPerPixel() const
  {
    return WrappedScanConverter->MmPerPixel;
  }

  double GetTrueDepth() const
  {
    return WrappedScanConverter->TrueDepth;
  }

  int GetWidthScan() const
  {
    return WrappedScanConverter->WidthScan;
  }

  int GetZeroOfYScale() const
  {
    return WrappedScanConverter->ZeroOfYScale;
  }

  Container::ScanConverterError HardInitScanConverter( int depth,
    int widthScan, int heightScan, int steering, int depthCfm )
  {
    
    return static_cast< Container::ScanConverterError >(
      WrappedScanConverter->HardInitScanConverter( depth, widthScan,
        heightScan, steering, depthCfm, WrappedCapture.get(),
        WrappedImageBuilding.get() ) );
  }

  Container::ScanConverterError IdleInitScanConverter( int depth,
    int widthScan, int heightScan, short idleId, int idleSteering, int depthCfm,
    bool idleDoubler, bool idleCompound, int idleCompoundAngle, bool idleCfm )
  {
    return static_cast< Container::ScanConverterError >(
      WrappedScanConverter->IdleInitScanConverter( depth, widthScan,
        heightScan, idleId, idleSteering, depthCfm, idleDoubler, idleCompound,
        idleCompoundAngle, idleCfm, WrappedImageBuilding.get() ) );
  }

  // 
  // Begin Capture
  //
  bool GetFrameAvg()
  {
    return WrappedCapture->FrameAvg;
  }

  void SetFrameAvg( bool value)
  {
    WrappedCapture->FrameAvg = value;
  }

  bool GetRFData()
  {
    return WrappedCapture->RFData;
  }

  void SetRFData( bool value )
  {
    if (value){
      this->hwControls->ReadFPGAVersion();
    }
    WrappedCapture->RFData = value;
    //Set correct image handler
    if (value){
      WrappedCapture->NewImageTick -= HandlerDelegate;
      WrappedCapture->NewImageTick += RFHandlerDelegate;
    }
    else{
      WrappedCapture->NewImageTick -= RFHandlerDelegate;
      WrappedCapture->NewImageTick += HandlerDelegate;
    }
  }

  double GetScanOn()
  {
    return WrappedCapture->ScanOn;
  }

  void StartReadScan()
  {
    WrappedCapture->StartReadScan( (ArrayType ^)Buffer );
  }

  void StartRFReadScan()
  {
    WrappedCapture->StartRFReadScan( (RFArrayType ^)RFBuffer );
  }

  void StopReadScan()
  {
    WrappedCapture->StopReadScan();
  }

  void SetNewImageCallback( NewImageCallbackType callback,
    void *clientData = 0 )
  {
    this->Handler->SetNewImageCallback( callback, clientData );
  }

  void SetNewRFImageCallback( NewRFImageCallbackType callback,
    void *clientData = 0 )
  {
    this->RFHandler->SetNewRFImageCallback( callback, clientData );
  }
  void SetHWControls(IntersonArrayCxx::Controls::HWControls * controls)
  {
    this->hwControls = controls;
  }

  // 
  // Begin Wrapped ImageBuilding
  //

private:
  IntersonArrayCxx::Controls::HWControls * hwControls;
  msclr::auto_gcroot< IntersonArray::Imaging::ScanConverter ^ >
    WrappedScanConverter;
  msclr::auto_gcroot< IntersonArray::Imaging::ImageBuilding ^ >
    WrappedImageBuilding;
  msclr::auto_gcroot< IntersonArray::Imaging::Capture ^ >
    WrappedCapture;

  gcroot< ArrayType ^ >                         Buffer;
  gcroot< RFArrayType ^ >                       RFBuffer;
  gcroot< NewImageHandler ^ >                   Handler;
  gcroot< NewRFImageHandler ^ >                 RFHandler;

  gcroot< IntersonArray::Imaging::Capture::NewImageHandler ^ >
    HandlerDelegate;
  gcroot< IntersonArray::Imaging::Capture::NewImageHandler ^ >
    RFHandlerDelegate;

};


#pragma unmanaged

Container
::Container():
  Impl( new ContainerImpl() )
{
}


Container
::~Container()
{
  delete Impl;
}


bool
Container
::GetCompound()
{
  return Impl->GetCompound();
}


void
Container
::SetCompound( bool value )
{
  Impl->SetCompound( value );
}


bool
Container
::GetDoubler()
{
  return Impl->GetDoubler();
}


void
Container
::SetDoubler( bool value )
{
  Impl->SetDoubler( value );
}


int
Container
::GetHeightScan() const
{
  return Impl->GetHeightScan();
}


float
Container
::GetMmPerPixel() const
{
  return Impl->GetMmPerPixel();
}


double
Container
::GetTrueDepth() const
{
  return Impl->GetMmPerPixel();
}


int
Container
::GetWidthScan() const
{
  return Impl->GetWidthScan();
}


int
Container
::GetZeroOfYScale() const
{
  return Impl->GetZeroOfYScale();
}


Container::ScanConverterError
Container
::HardInitScanConverter( int depth, int widthScan, int heightScan,
  int steering, int depthCfm )
{
  return Impl->HardInitScanConverter( depth, widthScan, heightScan,
    steering, depthCfm );
}


Container::ScanConverterError
Container
::IdleInitScanConverter( int depth, int width, int height, short idleId,
  int idleSteering, int depthCfm, bool idleDoubler, bool idleCompound,
  int idleCompoundAngle, bool idleCfm )
{
  return Impl->IdleInitScanConverter( depth, width, height, idleId,
    idleSteering, depthCfm, idleDoubler, idleCompound, idleCompoundAngle,
    idleCfm );
}

//
// Begin Capture
//
bool
Container
::GetRFData()
{
  return Impl->GetRFData();
}

void
Container
::SetRFData( bool transferOn )
{
  Impl->SetRFData( transferOn );
}


bool
Container
::GetFrameAvg()
{
  return Impl->GetFrameAvg();
}

void
Container
::SetFrameAvg( bool doAveraging )
{
  Impl->SetFrameAvg( doAveraging );
}


bool
Container
::GetScanOn() const
{
  return Impl->GetScanOn();
}


void
Container
::StartReadScan()
{
  Impl->StartReadScan();
}


void
Container
::StartRFReadScan()
{
  Impl->StartRFReadScan();
}


void
Container
::StopReadScan()
{
  Impl->StopReadScan();
}


void
Container
::SetNewImageCallback( NewImageCallbackType callback,
  void *clientData )
{
  Impl->SetNewImageCallback( callback, clientData );
}


void
Container
::SetNewRFImageCallback( NewRFImageCallbackType callback,
  void *clientData )
{
  Impl->SetNewRFImageCallback( callback, clientData );
}

void
Container
::SetHWControls(IntersonArrayCxx::Controls::HWControls * controls)
{
  Impl->SetHWControls(controls);
}

} // end namespace Imaging

} // end namespace IntersonArrayCxx
