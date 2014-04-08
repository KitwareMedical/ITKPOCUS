#pragma unmanaged
#include "IntersonCxxImagingScan2DClass.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"
// For StartReadScan
#using "System.Drawing.dll"

namespace IntersonCxx
{

namespace Imaging
{

class Scan2DClassImpl
{
public:
  typedef cli::array< byte, 2 > BmodeArrayType;

  typedef Scan2DClass::NewBmodeImageCallbackType NewBmodeImageCallbackType;

  Scan2DClassImpl()
    {
    Wrapped = gcnew Interson::Imaging::Scan2DClass();
    BmodeBuffer = gcnew BmodeArrayType( Scan2DClass::MAX_VECTORS, Scan2DClass::MAX_SAMPLES );
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

  void StartReadScan( unsigned char * buffer )
    {
    Wrapped->StartReadScan( (BmodeArrayType ^)BmodeBuffer );
    for( int ii = 0; ii < Scan2DClass::MAX_VECTORS; ++ii )
      {
      for( int jj = 0; jj < Scan2DClass::MAX_SAMPLES; ++jj )
         {
         buffer[Scan2DClass::MAX_SAMPLES * ii + jj] = BmodeBuffer[ii, jj];
         }
      }
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

 void SetNewBmodeImageCallback( NewBmodeImageCallbackType callback, void * clientData = 0)
    {
    this->NewBmodeImageCallback = callback;
    this->NewBmodeImageCallbackClientData = clientData;
    }

private:
  gcroot< Interson::Imaging::Scan2DClass ^ > Wrapped;

  gcroot< BmodeArrayType ^ > BmodeBuffer;

  unsigned char * NativeBmodeBuffer;
  NewBmodeImageCallbackType NewBmodeImageCallback;
  void * NewBmodeImageCallbackClientData;
};


#pragma unmanaged

Scan2DClass
::Scan2DClass():
  Impl( new Scan2DClassImpl() ),
  BmodeBuffer( new unsigned char[MAX_SAMPLES * MAX_VECTORS] )
{
  //this->Impl->SetInterface( this );
}



Scan2DClass
::~Scan2DClass()
{
  delete Impl;
  delete [] BmodeBuffer;
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
  Impl->StartReadScan( BmodeBuffer );
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
