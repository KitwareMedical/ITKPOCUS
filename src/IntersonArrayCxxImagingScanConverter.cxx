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
#include "IntersonArrayCxxImagingScanConverter.h"
#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include <msclr/auto_gcroot.h>

#using "IntersonArray.dll"

namespace IntersonArrayCxx
{

namespace Imaging
{

class ScanConverterImpl
{
public:
  ScanConverterImpl()
  {
    Wrapped = gcnew IntersonArray::Imaging::ScanConverter();
  }

  ~ScanConverterImpl()
  {
  }

  bool GetCompound()
  {
    return Wrapped->Compound;
  }

  void SetCompound( bool value )
  {
    Wrapped->Compound = value;
  }

  bool GetDoubler()
  {
    return Wrapped->Doubler;
  }

  void SetDoubler( bool value )
  {
    Wrapped->Doubler = value;
  }

  int GetHeightScan() const
  {
    return Wrapped->HeightScan;
  }

  float GetMmPerPixel() const
  {
    return Wrapped->MmPerPixel;
  }

  double GetTrueDepth() const
  {
    return Wrapped->TrueDepth;
  }

  int GetWidthScan() const
  {
    return Wrapped->WidthScan;
  }

  int GetZeroOfYScale() const
  {
    return Wrapped->ZeroOfYScale;
  }

  ScanConverter::ScanConverterError HardInitScanConverter( int depth,
    int widthScan, int heightScan, int steering, Capture & capture,
    ImageBuilding & imageBuilding )
  {
    msclr::auto_gcroot< IntersonArray::Imaging::Capture ^ >
      captureWrapped;
    captureWrapped = gcnew IntersonArray::Imaging::Capture();
    captureWrapped->RFData = capture.GetRFData();
    captureWrapped->FrameAvg = capture.GetFrameAvg();

    msclr::auto_gcroot< IntersonArray::Imaging::ImageBuilding ^ >
      imageWrapped;
    imageWrapped = gcnew IntersonArray::Imaging::ImageBuilding();
    return static_cast< ScanConverter::ScanConverterError >( Wrapped->
      HardInitScanConverter( depth, widthScan, heightScan, steering,
      captureWrapped.get(), imageWrapped.get() ) );
  }

  ScanConverter::ScanConverterError IdleInitScanConverter( int depth,
    int widthScan, int heightScan, short idleId, int idleSteering,
    bool idleDoubler, bool idleCompound, int idleCompoundAngle,
    ImageBuilding & imageBuilding )
  {
    msclr::auto_gcroot< IntersonArray::Imaging::ImageBuilding ^ >
      imageWrapped;
    imageWrapped = gcnew IntersonArray::Imaging::ImageBuilding();
    return static_cast< ScanConverter::ScanConverterError >( Wrapped->
      IdleInitScanConverter( depth, widthScan, heightScan, idleId,
        idleSteering, idleDoubler, idleCompound, idleCompoundAngle,
        imageWrapped.get() ) );
  }

private:
  gcroot< IntersonArray::Imaging::ScanConverter ^ >
    Wrapped;
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


bool
ScanConverter
::GetCompound()
{
  return Impl->GetCompound();
}


void
ScanConverter
::SetCompound( bool value )
{
  Impl->SetCompound( value );
}


bool
ScanConverter
::GetDoubler()
{
  return Impl->GetDoubler();
}


void
ScanConverter
::SetDoubler( bool value )
{
  Impl->SetDoubler( value );
}


int
ScanConverter
::GetHeightScan() const
{
  return Impl->GetHeightScan();
}


float
ScanConverter
::GetMmPerPixel() const
{
  return Impl->GetMmPerPixel();
}


double
ScanConverter
::GetTrueDepth() const
{
  return Impl->GetMmPerPixel();
}


int
ScanConverter
::GetWidthScan() const
{
  return Impl->GetWidthScan();
}


int
ScanConverter
::GetZeroOfYScale() const
{
  return Impl->GetZeroOfYScale();
}


ScanConverter::ScanConverterError
ScanConverter
::HardInitScanConverter( int depth, int widthScan, int heightScan,
  int steering, Capture & capture, ImageBuilding & imageBuilding )
{
  return Impl->HardInitScanConverter( depth, widthScan, heightScan,
    steering, capture, imageBuilding );
}


ScanConverter::ScanConverterError
ScanConverter
::IdleInitScanConverter( int depth, int width, int height, short idleId,
  int idleSteering, bool idleDoubler, bool idleCompound,
  int idleCompoundAngle, ImageBuilding & imageBuilding )
{
  return Impl->IdleInitScanConverter( depth, width, height, idleId,
    idleSteering, idleDoubler, idleCompound, idleCompoundAngle,
    imageBuilding );
}

} // end namespace Imaging

} // end namespace IntersonArrayCxx
