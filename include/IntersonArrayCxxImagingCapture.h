/*=========================================================================

Library:   IntersonArraySDKCxx

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
#ifndef _IntersonArrayCxxImagingCapture_h
#define _IntersonArrayCxxImagingCapture_h

#include "IntersonArrayCxx_Export.h"

#include <string>
#include <vector>

namespace IntersonArrayCxx
{

namespace Imaging
{

class CaptureImpl;

class IntersonArrayCxx_EXPORT Capture
{
public:
  Capture();
  ~Capture();

  typedef unsigned char  PixelType;
  typedef unsigned short RFPixelType;

  bool GetRFData();

  void SetRFData( bool value );

  bool GetFrameAvg();

  void SetFrameAvg( bool value );

  bool GetScanOn() const;

  void AbortScan();

  void DisposeScan();

  void StartReadScan();

  void StartRFReadScan();

  void StopReadScan();

  typedef void (__stdcall *NewImageCallbackType)( PixelType *buffer,
    void *clientData );
  void SetNewImageCallback( NewImageCallbackType callback,
    void *clientData = NULL );

  typedef void (__stdcall *NewRFImageCallbackType)( RFPixelType *buffer,
    void *clientData );
  void SetNewRFImageCallback( NewRFImageCallbackType callback,
    void *clientData = NULL );

private:
  Capture( const Capture &);
  void operator=( const Capture &);

  CaptureImpl *Impl;
};

} // end namespace Imaging

} // end namespace IntersonArrayCxx


#endif // _IntersonArrayCxxImagingCapture_h
