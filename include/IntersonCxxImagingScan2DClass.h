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

  bool GetScanOn() const;

  bool GetRFData() const;
  void SetRFData( bool transferOn );

  void StartReadScan();
  void StopReadScan();

private:
  Scan2DClass( const Scan2DClass & );
  void operator=( const Scan2DClass & );

  Scan2DClassImpl * Impl;

  unsigned char * BmodeBuffer;
};

} // end namespace Imaging

} // end namespace IntersonCxx


#endif // _IntersonCxxImagingScan2DClass_h
