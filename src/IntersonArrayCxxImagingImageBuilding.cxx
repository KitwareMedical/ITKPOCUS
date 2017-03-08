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
#include "IntersonArrayCxxImagingImageBuilding.h"
#include <iostream>
#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "System.Drawing.dll"

#using "IntersonArray.dll"

namespace IntersonArrayCxx
{

namespace Imaging
{

class ImageBuildingImpl
{
public:
  ImageBuildingImpl()
  {
    Wrapped = gcnew IntersonArray::Imaging::ImageBuilding();
  }

  ~ImageBuildingImpl()
  {
  }

  void Build2D( unsigned char * bmpDest, unsigned char * byteRaw,
    ScanConverter & scanConverter )
  {
    std::cerr << "ERROR: Call to ImageBuilding.Build2D - not implemented"
      << std::endl;
    //Wrapped->Build2D( bmpDest, byteRaw, scanConverter );
  }

  void Build2D( unsigned char * bmpDest, unsigned char * byteRawCurrent,
    unsigned char * byteRawPrevious, ScanConverter & scanConverter )
  {
    std::cerr << "ERROR: Call to ImageBuilding.Build2D.2 - not implemented"
      << std::endl;
    //Wrapped->Build2D( bmpDest, byteRawCurrent, byteRawPrevious,
      //scanConverter );
  }

private:
  gcroot< IntersonArray::Imaging::ImageBuilding ^ >    Wrapped;

};


#pragma unmanaged

ImageBuilding
::ImageBuilding():
  Impl( new ImageBuildingImpl() )
{
}


ImageBuilding
::~ImageBuilding()
{
  delete Impl;
}


void
ImageBuilding
::Build2D( unsigned char * bmpDest, unsigned char * byteRaw,
  ScanConverter & scanConverter )
{
  return Impl->Build2D( bmpDest, byteRaw, scanConverter );
}

void
ImageBuilding
::Build2D( unsigned char * bmpDest, unsigned char * byteRawCurrent,
  unsigned char * byteRawPrevious, ScanConverter & scanConverter )
{
  return Impl->Build2D( bmpDest, byteRawCurrent, byteRawPrevious,
    scanConverter );
}

} // end namespace Imaging

} // end namespace IntersonArrayCxx
