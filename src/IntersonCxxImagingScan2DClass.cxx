#pragma unmanaged
#include "IntersonCxxImagingScan2DClass.h"

#include <iostream>

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"
// For StartReadScan
//#using "System.Drawing.dll"

namespace IntersonCxx
{

namespace Imaging
{

ref class NewBmodeImageHandler
{
public:
  typedef Scan2DClass::NewBmodeImageCallbackType NewBmodeImageCallbackType;
  typedef cli::array< byte, 2 > BmodeArrayType;

  NewBmodeImageHandler( BmodeArrayType ^ managedBmodeBuffer ):
      NewBmodeImageCallback( NULL ),
      NewBmodeImageCallbackClientData( NULL ),
      NativeBmodeBuffer( new unsigned char[Scan2DClass::MAX_SAMPLES * Scan2DClass::MAX_VECTORS] ),
      ManagedBmodeBuffer( managedBmodeBuffer )
    {
    }

  ~NewBmodeImageHandler()
    {
    delete [] NativeBmodeBuffer;
    }

  void HandleNewBmodeImage( Interson::Imaging::Scan2DClass ^ scan2D, System::EventArgs ^ eventArgs )
    {
    std::cout << "calling callback!" << std::endl;
    if( this->NewBmodeImageCallback != NULL )
      {
      for( int ii = 0; ii < Scan2DClass::MAX_VECTORS; ++ii )
        {
        for( int jj = 0; jj < Scan2DClass::MAX_SAMPLES; ++jj )
          {
          this->NativeBmodeBuffer[Scan2DClass::MAX_SAMPLES * ii + jj] = this->ManagedBmodeBuffer[ii, jj];
          }
        }
      }
      this->NewBmodeImageCallback( this->NativeBmodeBuffer, this->NewBmodeImageCallbackClientData );
    }

  void SetNewBmodeImageCallback( NewBmodeImageCallbackType callback, void * clientData )
    {
    this->NewBmodeImageCallback = callback;
    this->NewBmodeImageCallbackClientData = clientData;
    }

private:
  NewBmodeImageCallbackType NewBmodeImageCallback;
  void * NewBmodeImageCallbackClientData;
  unsigned char * NativeBmodeBuffer;
  BmodeArrayType ^ ManagedBmodeBuffer;
};

class Scan2DClassImpl
{
public:
  typedef cli::array< byte, 2 > BmodeArrayType;

  typedef Scan2DClass::NewBmodeImageCallbackType NewBmodeImageCallbackType;

  Scan2DClassImpl()
    {
    Wrapped = gcnew Interson::Imaging::Scan2DClass();
    BmodeBuffer = gcnew BmodeArrayType( Scan2DClass::MAX_VECTORS, Scan2DClass::MAX_SAMPLES );

    BmodeHandler = gcnew NewBmodeImageHandler( BmodeBuffer );
    BmodeHandlerDelegate = gcnew
        Interson::Imaging::Scan2DClass::NewImageHandler(BmodeHandler,
        &NewBmodeImageHandler::HandleNewBmodeImage );
    Wrapped->NewImageTick += BmodeHandlerDelegate;
    }

  ~Scan2DClassImpl()
    {
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

  void StartReadScan()
    {
    Wrapped->StartReadScan( (BmodeArrayType ^)BmodeBuffer );
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

private:
  gcroot< Interson::Imaging::Scan2DClass ^ > Wrapped;
  gcroot< BmodeArrayType ^ >                 BmodeBuffer;
  gcroot< NewBmodeImageHandler ^ >           BmodeHandler;
  gcroot< Interson::Imaging::Scan2DClass::NewImageHandler ^ > BmodeHandlerDelegate;

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


void
Scan2DClass
::StartReadScan()
{
  Impl->StartReadScan();
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

} // end namespace Imaging

} // end namespace IntersonCxx
