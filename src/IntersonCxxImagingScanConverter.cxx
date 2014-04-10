#pragma unmanaged
#include "IntersonCxxImagingScanConverter.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"

namespace IntersonCxx
{

namespace Imaging
{

class ScanConverterImpl
{
public:
  ScanConverterImpl()
    {
    Wrapped = gcnew Interson::Imaging::ScanConverter();
    }

  ~ScanConverterImpl()
    {
    }

  int GetHeightScan()
    {
    return Wrapped->HeightScan;
    }

  int GetWidthScan()
    {
    return Wrapped->WidthScan;
    }

  float GetMmPerPixel()
    {
    return Wrapped->MmPerPixel;
    }

  int GetZeroOfYScale()
    {
    return Wrapped->ZeroOfYScale;
    }


private:
  gcroot< Interson::Imaging::ScanConverter ^ >    Wrapped;

};


#pragma unmanaged

ScanConverter
::ScanConverter():
  Impl( new ScanConverterImpl() )
{
}


ScanConverter
::~ScanConverter()
{
  delete Impl;
}


int
ScanConverter
::GetHeightScan() const
{
  return Impl->GetHeightScan();
}


int
ScanConverter
::GetWidthScan() const
{
  return Impl->GetWidthScan();
}


float
ScanConverter
::GetMmPerPixel() const
{
  return Impl->GetMmPerPixel();
}


int
ScanConverter
::GetZeroOfYScale() const
{
  return Impl->GetZeroOfYScale();
}

} // end namespace Imaging

} // end namespace IntersonCxx
