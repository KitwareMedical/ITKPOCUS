#pragma unmanaged
#include "IntersonCxxImagingScan2DClass.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"
// for Bitmap
#using "System.Drawing.dll"

namespace IntersonCxx
{

namespace Imaging
{

ref class NewRFImageHandler
{
public:
  typedef Scan2DClass::NewRFImageCallbackType            NewRFImageCallbackType;
  typedef IntersonCxx::Imaging::Scan2DClass::RFPixelType RFPixelType;
  typedef cli::array< unsigned short, 2 >                RFArrayType;

  NewRFImageHandler( RFArrayType ^ managedRFBuffer ):
      NewRFImageCallback( NULL ),
      NewRFImageCallbackClientData( NULL ),
      NativeRFBuffer( new RFPixelType[Scan2DClass::MAX_RFSAMPLES * Scan2DClass::MAX_VECTORS] ),
      ManagedRFBuffer( managedRFBuffer )
    {
    }

  ~NewRFImageHandler()
    {
    delete [] NativeRFBuffer;
    }

  void HandleNewRFImage( Interson::Imaging::Scan2DClass ^ scan2D, System::EventArgs ^ eventArgs )
    {
    if( this->NewRFImageCallback != NULL )
      {
      for( int ii = 0; ii < Scan2DClass::MAX_VECTORS; ++ii )
        {
        for( int jj = 0; jj < Scan2DClass::MAX_RFSAMPLES; ++jj )
          {
          this->NativeRFBuffer[Scan2DClass::MAX_RFSAMPLES * ii + jj] = this->ManagedRFBuffer[ii, jj];
          }
        }
      this->NewRFImageCallback( this->NativeRFBuffer, this->NewRFImageCallbackClientData );
      }
    }

  void SetNewRFImageCallback( NewRFImageCallbackType callback, void * clientData )
    {
    this->NewRFImageCallback = callback;
    this->NewRFImageCallbackClientData = clientData;
    }

private:
  NewRFImageCallbackType NewRFImageCallback;
  void * NewRFImageCallbackClientData;
  RFPixelType * NativeRFBuffer;
  RFArrayType ^ ManagedRFBuffer;
};


ref class NewBmodeImageHandler
{
public:
  typedef Scan2DClass::NewBmodeImageCallbackType            NewBmodeImageCallbackType;
  typedef Scan2DClass::NewScanConvertedBmodeImageCallbackType
                                                            NewScanConvertedBmodeImageCallbackType;
  typedef IntersonCxx::Imaging::Scan2DClass::BmodePixelType BmodePixelType;
  typedef cli::array< byte, 2 >                             BmodeArrayType;

  NewBmodeImageHandler( BmodeArrayType ^ managedBmodeBuffer ):
      NewBmodeImageCallback( NULL ),
      NewBmodeImageCallbackClientData( NULL ),
      NativeBmodeBuffer( new BmodePixelType[Scan2DClass::MAX_SAMPLES * Scan2DClass::MAX_VECTORS] ),
      ManagedBmodeBuffer( managedBmodeBuffer ),
      NewScanConvertedBmodeImageCallback( NULL ),
      NewScanConvertedBmodeImageCallbackClientData( NULL ),
      ScanConvertedBmodeBuffer( new BmodePixelType[0] )
    {
    ScanConverter = gcnew Interson::Imaging::ScanConverter();
    }

  ~NewBmodeImageHandler()
    {
    delete [] NativeBmodeBuffer;
    delete [] ScanConvertedBmodeBuffer;
    }

  void HandleNewBmodeImage( Interson::Imaging::Scan2DClass ^ scan2D, System::EventArgs ^ eventArgs )
    {
    if( this->NewBmodeImageCallback != NULL )
      {
      for( int ii = 0; ii < Scan2DClass::MAX_VECTORS; ++ii )
        {
        for( int jj = 0; jj < Scan2DClass::MAX_SAMPLES; ++jj )
          {
          this->NativeBmodeBuffer[Scan2DClass::MAX_SAMPLES * ii + jj] =
            this->ManagedBmodeBuffer[ii, jj];
          }
        }
      this->NewBmodeImageCallback( this->NativeBmodeBuffer, this->NewBmodeImageCallbackClientData );
      }

    if( this->NewScanConvertedBmodeImageCallback != NULL )
      {
      scan2D->Build2D( ScanConvertedBmode, ManagedBmodeBuffer );
      for( int ii = 0; ii < ScanConvertedBmode->Height; ++ii )
        {
        for( int jj = 0; jj < ScanConvertedBmode->Width; ++jj )
          {
          // Assume it is grayscale, i.e. R = G = B
          this->ScanConvertedBmodeBuffer[ScanConvertedBmode->Width * ii + jj] =
            static_cast< BmodePixelType >( ScanConvertedBmode->GetPixel( jj, ii ).B );
          }
        }
      this->NewScanConvertedBmodeImageCallback( this->ScanConvertedBmodeBuffer, this->NewScanConvertedBmodeImageCallbackClientData );
      }
    }

  void SetNewBmodeImageCallback( NewBmodeImageCallbackType callback, void * clientData )
    {
    this->NewBmodeImageCallback = callback;
    this->NewBmodeImageCallbackClientData = clientData;
    }

  void SetNewScanConvertedBmodeImageCallback( NewScanConvertedBmodeImageCallbackType callback, void * clientData )
    {
    this->NewScanConvertedBmodeImageCallback = callback;
    this->NewScanConvertedBmodeImageCallbackClientData = clientData;

    ScanConvertedBmode = gcnew System::Drawing::Bitmap( ScanConverter->WidthScan,
                                                        ScanConverter->HeightScan,
                                                        System::Drawing::Imaging::PixelFormat::Format8bppIndexed );
    delete [] ScanConvertedBmodeBuffer;
    ScanConvertedBmodeBuffer = new BmodePixelType[ScanConverter->WidthScan * ScanConverter->HeightScan];
    }

private:
  NewBmodeImageCallbackType NewBmodeImageCallback;
  void * NewBmodeImageCallbackClientData;
  BmodePixelType * NativeBmodeBuffer;
  BmodeArrayType ^ ManagedBmodeBuffer;
  System::Drawing::Bitmap ^          ScanConvertedBmode;

  NewScanConvertedBmodeImageCallbackType NewScanConvertedBmodeImageCallback;
  void * NewScanConvertedBmodeImageCallbackClientData;
  Interson::Imaging::ScanConverter ^ ScanConverter;
  BmodePixelType * ScanConvertedBmodeBuffer;
};


class Scan2DClassImpl
{
public:
  typedef cli::array< byte, 2 >           BmodeArrayType;
  typedef cli::array< unsigned short, 2 > RFArrayType;

  typedef Scan2DClass::NewBmodeImageCallbackType NewBmodeImageCallbackType;
  typedef Scan2DClass::NewScanConvertedBmodeImageCallbackType
                                                 NewScanConvertedBmodeImageCallbackType;
  typedef Scan2DClass::NewRFImageCallbackType    NewRFImageCallbackType;

  Scan2DClassImpl()
    {
    Wrapped = gcnew Interson::Imaging::Scan2DClass();

    BmodeBuffer = gcnew BmodeArrayType( Scan2DClass::MAX_VECTORS, Scan2DClass::MAX_SAMPLES );
    BmodeHandler = gcnew NewBmodeImageHandler( BmodeBuffer );
    BmodeHandlerDelegate = gcnew
        Interson::Imaging::Scan2DClass::NewImageHandler(BmodeHandler,
        &NewBmodeImageHandler::HandleNewBmodeImage );
    Wrapped->NewImageTick += BmodeHandlerDelegate;

    RFBuffer = gcnew RFArrayType( Scan2DClass::MAX_VECTORS, Scan2DClass::MAX_RFSAMPLES );
    RFHandler = gcnew NewRFImageHandler( RFBuffer );
    RFHandlerDelegate = gcnew
        Interson::Imaging::Scan2DClass::NewImageHandler(RFHandler,
        &NewRFImageHandler::HandleNewRFImage );
    Wrapped->NewImageTick += RFHandlerDelegate;
    }

  ~Scan2DClassImpl()
    {
    Wrapped->NewImageTick -= RFHandlerDelegate;
    Wrapped->NewImageTick -= BmodeHandlerDelegate;
    }

  bool GetScanOn()
    {
    return Wrapped->ScanOn;
    }

  bool GetRFData()
    {
    return Wrapped->RFData;
    }

  void SetRFData( bool transferOn )
    {
    Wrapped->RFData = transferOn;
    }

  bool GetFrameAvg()
    {
    return Wrapped->FrameAvg;
    }

  void SetFrameAvg( bool doAveraging )
    {
    return Wrapped->FrameAvg = doAveraging;
    }

  double GetTrueDepth()
    {
    return Wrapped->TrueDepth;
    }

  void StartReadScan()
    {
    Wrapped->StartReadScan( (BmodeArrayType ^)BmodeBuffer );
    }

  void StartRFReadScan()
    {
    Wrapped->StartRFReadScan( (RFArrayType ^)RFBuffer );
    }

  void StopReadScan()
    {
    Wrapped->StopReadScan();
    }

  void DisposeScan()
    {
    Wrapped->DisposeScan();
    }

  void AbortScan()
    {
    Wrapped->AbortScan();
    }

  void SetNewBmodeImageCallback( NewBmodeImageCallbackType callback, void * clientData = 0 )
    {
    this->BmodeHandler->SetNewBmodeImageCallback( callback, clientData );
    }

  void SetNewScanConvertedBmodeImageCallback( NewScanConvertedBmodeImageCallbackType callback, void * clientData = 0 )
    {
    this->BmodeHandler->SetNewScanConvertedBmodeImageCallback( callback, clientData );
    }

  void SetNewRFImageCallback( NewRFImageCallbackType callback, void * clientData = 0 )
    {
    this->RFHandler->SetNewRFImageCallback( callback, clientData );
    }


private:
  gcroot< Interson::Imaging::Scan2DClass ^ >                  Wrapped;

  gcroot< BmodeArrayType ^ >                                  BmodeBuffer;
  gcroot< RFArrayType ^ >                                     RFBuffer;
  gcroot< NewBmodeImageHandler ^ >                            BmodeHandler;
  gcroot< NewRFImageHandler ^ >                               RFHandler;
  gcroot< Interson::Imaging::Scan2DClass::NewImageHandler ^ > BmodeHandlerDelegate;
  gcroot< Interson::Imaging::Scan2DClass::NewImageHandler ^ > RFHandlerDelegate;

};


#pragma unmanaged

Scan2DClass
::Scan2DClass():
  Impl( new Scan2DClassImpl() )
{
}


Scan2DClass
::~Scan2DClass()
{
  delete Impl;
}


bool
Scan2DClass
::GetScanOn() const
{
  return Impl->GetScanOn();
}


bool
Scan2DClass
::GetRFData() const
{
  return Impl->GetRFData();
}


void
Scan2DClass
::SetRFData( bool transferOn )
{
  Impl->SetRFData( transferOn );
}


bool
Scan2DClass
::GetFrameAvg() const
{
  return Impl->GetFrameAvg();
}

void
Scan2DClass
::SetFrameAvg( bool doAveraging )
{
  Impl->SetFrameAvg( doAveraging );
}


void
Scan2DClass
::StartReadScan()
{
  Impl->StartReadScan();
}


void
Scan2DClass
::StartRFReadScan()
{
  Impl->StartRFReadScan();
}


void
Scan2DClass
::StopReadScan()
{
  Impl->StopReadScan();
}


void
Scan2DClass
::DisposeScan()
{
  Impl->DisposeScan();
}


void
Scan2DClass
::AbortScan()
{
  Impl->AbortScan();
}


void
Scan2DClass
::SetNewBmodeImageCallback( NewBmodeImageCallbackType callback,
                            void * clientData )
{
  Impl->SetNewBmodeImageCallback( callback, clientData );
}


void
Scan2DClass
::SetNewScanConvertedBmodeImageCallback( NewScanConvertedBmodeImageCallbackType callback,
                            void * clientData )
{
  Impl->SetNewScanConvertedBmodeImageCallback( callback, clientData );
}


void
Scan2DClass
::SetNewRFImageCallback( NewRFImageCallbackType callback,
                            void * clientData )
{
  Impl->SetNewRFImageCallback( callback, clientData );
}


double
Scan2DClass
::GetTrueDepth() const
{
  return Impl->GetTrueDepth();
}

} // end namespace Imaging

} // end namespace IntersonCxx
