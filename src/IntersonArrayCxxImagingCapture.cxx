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
#pragma unmanaged
#include "IntersonArrayCxxImagingCapture.h"
#include "IntersonArrayCxxImagingScanConverter.h"
#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "IntersonArray.dll"

// for Bitmap
#using "System.Drawing.dll"

namespace IntersonArrayCxx
{

namespace Imaging
{

ref class NewRFImageHandler
{
public:

  typedef Capture::NewRFImageCallbackType   NewRFImageCallbackType;

  typedef IntersonArrayCxx::Imaging::Capture::RFPixelType   RFPixelType;
  typedef cli::array< RFPixelType, 2 >                      RFArrayType;

  NewRFImageHandler( RFArrayType ^managedRFBuffer ):
    NewRFImageCallback( NULL ),
    NewRFImageCallbackClientData( NULL ),
    NativeRFBuffer( new RFPixelType[ScanConverter::MAX_RFSAMPLES
      * 512] ),
      //* ScanConverter::MAX_VECTORS]
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
      //for ( int ii = 0; ii < ScanConverter::MAX_VECTORS; ++ii )
      for ( int ii = 0; ii < 512; ++ii )
      {
        for ( int jj = 0; jj < ScanConverter::MAX_RFSAMPLES; ++jj )
        {
          this->NativeRFBuffer[ScanConverter::MAX_RFSAMPLES * ii + jj] =
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
  RFPixelType            * NativeRFBuffer;
  RFArrayType             ^ManagedRFBuffer;
};


ref class NewImageHandler
{
public:
  typedef Capture::NewImageCallbackType              NewImageCallbackType;

  typedef IntersonArrayCxx::Imaging::Capture::PixelType     PixelType;
  typedef cli::array< byte, 2 >                             ArrayType;

  NewImageHandler( ArrayType ^managedBuffer ):
    NewImageCallback( NULL ),
    NewImageCallbackClientData( NULL ),
    NativeBuffer( new PixelType[ScanConverter::MAX_SAMPLES *
      512] ),
      //ScanConverter::MAX_VECTORS] ),
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
      for ( int ii = 0; ii < 512; ++ii )
      //for ( int ii = 0; ii < ScanConverter::MAX_VECTORS; ++ii )
      {
        for ( int jj = 0; jj < ScanConverter::MAX_SAMPLES; ++jj )
        {
          this->NativeBuffer[ScanConverter::MAX_SAMPLES * ii + jj] =
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
  ArrayType             ^ManagedBuffer;
};


class CaptureImpl
{
public:
  typedef cli::array< Capture::PixelType, 2 >      ArrayType;
  typedef cli::array< Capture::RFPixelType, 2 >    RFArrayType;

  typedef Capture::NewImageCallbackType      NewImageCallbackType;
  typedef Capture::NewRFImageCallbackType    NewRFImageCallbackType;

  CaptureImpl()
  {
    Wrapped = gcnew IntersonArray::Imaging::Capture();

    //Buffer = gcnew ArrayType( ScanConverter::MAX_VECTORS,
    Buffer = gcnew ArrayType( 512,
      ScanConverter::MAX_SAMPLES);
    Handler = gcnew NewImageHandler( Buffer );
    HandlerDelegate = gcnew
      IntersonArray::Imaging::Capture::NewImageHandler( Handler,
        & NewImageHandler::HandleNewImage );
    Wrapped->NewImageTick += HandlerDelegate;

    //RFBuffer = gcnew RFArrayType( ScanConverter::MAX_VECTORS,
    RFBuffer = gcnew RFArrayType( 512,
      ScanConverter::MAX_RFSAMPLES );
    RFHandler = gcnew NewRFImageHandler( RFBuffer );
    RFHandlerDelegate = gcnew
      IntersonArray::Imaging::Capture::NewImageHandler( RFHandler,
        & NewRFImageHandler::HandleNewRFImage );
    Wrapped->NewImageTick += RFHandlerDelegate;

  }

  ~CaptureImpl()
  {
    Wrapped->NewImageTick -= RFHandlerDelegate;
    Wrapped->NewImageTick -= HandlerDelegate;
  }

  bool GetFrameAvg()
  {
    return Wrapped->FrameAvg;
  }

  void SetFrameAvg( bool value)
  {
    Wrapped->FrameAvg = value;
  }

  bool GetRFData()
  {
    return Wrapped->RFData;
  }

  void SetRFData( bool value )
  {
    Wrapped->RFData = value;
  }

  double GetScanOn()
  {
    return Wrapped->ScanOn;
  }

  void AbortScan()
  {
    Wrapped->AbortScan();
  }

  void DisposeScan()
  {
    Wrapped->DisposeScan();
  }

  void StartReadScan()
  {
    Wrapped->StartReadScan( (ArrayType ^)Buffer );
  }

  void StartRFReadScan()
  {
    Wrapped->StartRFReadScan( (RFArrayType ^)RFBuffer );
  }

  void StopReadScan()
  {
    Wrapped->StopReadScan();
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


private:
  gcroot< IntersonArray::Imaging::Capture ^ >   Wrapped;

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

Capture
::Capture():
  Impl( new CaptureImpl() )
{
}


Capture
::~Capture()
{
  delete Impl;
}


bool
Capture
::GetRFData()
{
  return Impl->GetRFData();
}


void
Capture
::SetRFData( bool transferOn )
{
  Impl->SetRFData( transferOn );
}


bool
Capture
::GetFrameAvg()
{
  return Impl->GetFrameAvg();
}

void
Capture
::SetFrameAvg( bool doAveraging )
{
  Impl->SetFrameAvg( doAveraging );
}


bool
Capture
::GetScanOn() const
{
  return Impl->GetScanOn();
}


void
Capture
::AbortScan()
{
  Impl->AbortScan();
}


void
Capture
::DisposeScan()
{
  Impl->DisposeScan();
}


void
Capture
::StartReadScan()
{
  Impl->StartReadScan();
}


void
Capture
::StartRFReadScan()
{
  Impl->StartRFReadScan();
}


void
Capture
::StopReadScan()
{
  Impl->StopReadScan();
}


void
Capture
::SetNewImageCallback( NewImageCallbackType callback,
  void *clientData )
{
  Impl->SetNewImageCallback( callback, clientData );
}


void
Capture
::SetNewRFImageCallback( NewRFImageCallbackType callback,
  void *clientData )
{
  Impl->SetNewRFImageCallback( callback, clientData );
}


} // end namespace Imaging

} // end namespace IntersonArrayCxx
