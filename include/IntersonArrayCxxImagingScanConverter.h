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
#ifndef _IntersonArrayCxxImagingScanConverter_h
#define _IntersonArrayCxxImagingScanConverter_h

#include "IntersonArrayCxx_Export.h"
#include "IntersonArrayCxxImagingCapture.h"
#include "IntersonArrayCxxImagingImageBuilding.h"

namespace IntersonArrayCxx
{

namespace Imaging
{

class ScanConverterImpl;

class IntersonArrayCxx_EXPORT ScanConverter
{
public:
    ScanConverter();
    ~ScanConverter();

    static const int MAX_SAMPLES = 1024;
    static const int MAX_RFSAMPLES = 2048;

    bool GetCompound();

    void SetCompound( bool value );

    bool GetDoubler();

    void SetDoubler( bool value );

    int GetHeightScan() const;

    float GetMmPerPixel() const;

    double GetTrueDepth() const;

    int GetWidthScan() const;

    int GetZeroOfYScale() const;

    // Error code of HardInitScanConverter and SoftInitScanConverter
    enum ScanConverterError
    {
        SUCCESS = 1,
        PROBE_NOT_INITIALIZED, // FindMyProbe has not been called
        UNKNOWN_PROBE, // Probe Identity not valid
        UNDER_LIMITS, // Width * Height over 10 * 10
        OVER_LIMITS, // Width * Height over 800 * 1000
        WRONG_FORMAT, // other error
        ERROR
    };

    ScanConverterError HardInitScanConverter( int depth, int widthScan,
      int heightScan, int steering, Capture & capture,
      ImageBuilding & imageBuilding );

    ScanConverterError IdleInitScanConverter( int depth, int width,
      int height, short idleId, int idleSteering, bool idleDoubler,
      bool idleCompound, int idleCompoundAngle, ImageBuilding &
      imageBuilding );

private:
    ScanConverter( const ScanConverter &);
    void operator=( const ScanConverter &);

    ScanConverterImpl *Impl;
};

} // end namespace Imaging

} // end namespace IntersonArrayCxx


#endif // _IntersonArrayCxxImagingScanConverter_h
