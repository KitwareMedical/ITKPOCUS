#ifndef _IntersonCxxControlsHWControls_h
#define _IntersonCxxControlsHWControls_h

#include "IntersonCxx_Export.h"

namespace IntersonCxx
{

namespace Controls
{

class HWControlsImpl;

class IntersonCxx_EXPORT HWControls
{
public:
  HWControls();
  ~HWControls();

  static const int DIG_GAIN_MIN = -127;
  static const int DIG_GAIN_INIT = 0;
  static const int DIG_GAIN_MAX = 127;

  static const int MAX_FREQ = 3;

  static const int ID_GP_3_5MHz = 1;
  static const int ID_VC_7_5MHz = 2;
  static const int ID_SP_7_5MHz = 3;
  static const int ID_EC_7_5MHz = 4;
  static const int ID_MV_12_MHz = 5;
  static const int ID_GV_5_MHz = 6;

  static const unsigned short int EEOEMSpace = 4096;

  unsigned char GetProbeID() const;

private:
  HWControls( const HWControls & );
  void operator=( const HWControls & );

  HWControlsImpl * Impl;
};

} // end namespace Controls

} // end namespace IntersonCxx


#endif // _IntersonCxxControlsHWControls_h
