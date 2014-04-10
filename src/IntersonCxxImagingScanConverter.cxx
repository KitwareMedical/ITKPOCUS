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

  ScanConverter::ScanConverterError HardInitScanConverter( int depth,
                                                           bool upDown,
                                                           bool leftRight,
                                                           int widthScan,
                                                           int heightScan )
    {
    return static_cast< ScanConverter::ScanConverterError >(
      Wrapped->HardInitScanConverter( depth, upDown, leftRight, widthScan, heightScan ) );
    }

  ScanConverter::ScanConverterError IdleInitScanConverter( int depth,
                                                           bool upDown,
                                                           bool leftRight,
                                                           int widthScan,
                                                           int heightScan,
                                                           unsigned char probeId )
    {
    return static_cast< ScanConverter::ScanConverterError >(
      Wrapped->IdleInitScanConverter( depth, upDown, leftRight, widthScan, heightScan, probeId ) );
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


ScanConverter::ScanConverterError
ScanConverter
::HardInitScanConverter( int depth,
                         bool upDown,
                         bool leftRight,
                         int widthScan,
                         int heightScan )
{
  return Impl->HardInitScanConverter( depth,
                                      upDown,
                                      leftRight,
                                      widthScan,
                                      heightScan );
}


ScanConverter::ScanConverterError
ScanConverter
::IdleInitScanConverter( int depth,
                         bool upDown,
                         bool leftRight,
                         int widthScan,
                         int heightScan,
                         unsigned char probeId )
{
  return Impl->IdleInitScanConverter( depth,
                                      upDown,
                                      leftRight,
                                      widthScan,
                                      heightScan,
                                      probeId );
}

} // end namespace Imaging

} // end namespace IntersonCxx
