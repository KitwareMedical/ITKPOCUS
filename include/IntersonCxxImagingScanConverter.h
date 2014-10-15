#ifndef _IntersonCxxImagingScanConverter_h
#define _IntersonCxxImagingScanConverter_h

#include "IntersonCxx_Export.h"

namespace IntersonCxx
{

namespace Imaging
{

class ScanConverterImpl;

class IntersonCxx_EXPORT ScanConverter
{
public:
  ScanConverter();
  ~ScanConverter();

  static const int MAX_SAMPLES = 1024;
  static const int MAX_RFSAMPLES = 2048;
  static const int MAX_VECTORS = 241;

  // Error code of HardInitScanConverter and SoftInitScanConverter
  enum ScanConverterError
    {
    SUCCESS = 1,
    PROBE_NOT_INITIALIZED, // FindMyProbe has not been called
    UNKNOWN_PROBE, // Probe Identity not valid
    UNDER_LIMITS, // Width * Height over 10 * 10
    OVER_LIMITS, // Width * Height over 800 * 1000
    OTHER_ERROR // other error
    };

  int GetHeightScan() const;
  int GetWidthScan() const;

  float GetMmPerPixel() const;
  int GetZeroOfYScale() const;

  // Optimal iHeightScan is 512. Optimal iWidthScan is 776 for 90 degree (GP, SP, EC) probes, and 590 for a 60 degree (VC) probe.
  ScanConverterError HardInitScanConverter( int depth,
                                            bool upDown,
                                            bool leftRight,
                                            int widthScan,
                                            int heightScan );

  ScanConverterError IdleInitScanConverter( int depth,
                                            bool upDown,
                                            bool leftRight,
                                            int widthScan,
                                            int heightScan,
                                            unsigned char probeId );

private:
  ScanConverter( const ScanConverter & );
  void operator=( const ScanConverter & );

  ScanConverterImpl * Impl;
};

} // end namespace Imaging

} // end namespace IntersonCxx


#endif // _IntersonCxxImagingScanConverter_h
