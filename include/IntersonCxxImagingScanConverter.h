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


private:
  ScanConverter( const ScanConverter & );
  void operator=( const ScanConverter & );

  ScanConverterImpl * Impl;
};

} // end namespace Imaging

} // end namespace IntersonCxx


#endif // _IntersonCxxImagingScanConverter_h
