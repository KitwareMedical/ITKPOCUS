#ifndef _IntersonCxxImagingScan2DClass_h
#define _IntersonCxxImagingScan2DClass_h

#include "IntersonCxx_Export.h"

#include <string>
#include <vector>

namespace IntersonCxx
{

namespace Imaging
{

class Scan2DClassImpl;

class IntersonCxx_EXPORT Scan2DClass
{
public:
  Scan2DClass();
  ~Scan2DClass();

  static const int MAX_SAMPLES = 1024;
  static const int MAX_RFSAMPLES = 2048;
  static const int MAX_VECTORS = 241;

  typedef unsigned char  BmodePixelType;
  typedef short          RFPixelType;

  bool GetScanOn() const;

  bool GetRFData() const;
  void SetRFData( bool transferOn );

  bool GetFrameAvg() const;
  void SetFrameAvg( bool doAveraging );

  double GetTrueDepth() const;

  void StartReadScan();
  void StartRFReadScan();
  void StopReadScan();
  void DisposeScan();
  void AbortScan();

  typedef void (__stdcall * NewBmodeImageCallbackType)( BmodePixelType * buffer,
    void * clientData );
  void SetNewBmodeImageCallback( NewBmodeImageCallbackType callback,
    void * clientData = NULL );

  typedef void (__stdcall * NewScanConvertedBmodeImageCallbackType)( BmodePixelType * buffer,
    void * clientData );
  /** \warning The WidthScan and HeightScan must be initialized on the
   * ScanConverter before calling this method. */
  void SetNewScanConvertedBmodeImageCallback( NewScanConvertedBmodeImageCallbackType callback,
    void * clientData = NULL );

  typedef void (__stdcall * NewRFImageCallbackType)( RFPixelType * buffer,
    void * clientData );
  void SetNewRFImageCallback( NewRFImageCallbackType callback,
    void * clientData = NULL );

private:
  Scan2DClass( const Scan2DClass & );
  void operator=( const Scan2DClass & );

  Scan2DClassImpl * Impl;
};

} // end namespace Imaging

} // end namespace IntersonCxx


#endif // _IntersonCxxImagingScan2DClass_h
