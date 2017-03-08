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
#ifndef _IntersonArrayCxxImagingImageBuilding_h
#define _IntersonArrayCxxImagingImageBuilding_h

#include "IntersonArrayCxx_Export.h"

namespace IntersonArrayCxx
{

namespace Imaging
{

class ImageBuildingImpl;

class ScanConverter;

class IntersonArrayCxx_EXPORT ImageBuilding
{
public:
    ImageBuilding();
    ~ImageBuilding();

    void Build2D( unsigned char * bmpDest, unsigned char * byteRaw,
      ScanConverter & scanConverter );

    void Build2D( unsigned char * bmpDest, unsigned char * byteRawCurrent,
      unsigned char * byteRawPrevious, ScanConverter & scanConverter );

private:
    ImageBuilding( const ImageBuilding &);
    void operator=( const ImageBuilding &);

    ImageBuildingImpl *Impl;
};

} // end namespace Imaging

} // end namespace IntersonArrayCxx


#endif // _IntersonArrayCxxImagingImageBuilding_h
