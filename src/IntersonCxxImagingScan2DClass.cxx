#pragma unmanaged
#include "IntersonCxxImagingScan2DClass.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"

namespace IntersonCxx
{

namespace Imaging
{

class Scan2DClassImpl
{
public:
  Scan2DClassImpl()
    {
    Wrapped = gcnew Interson::Imaging::Scan2DClass();
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


private:
  gcroot< Interson::Imaging::Scan2DClass ^ > Wrapped;
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

} // end namespace Imaging

} // end namespace IntersonCxx
