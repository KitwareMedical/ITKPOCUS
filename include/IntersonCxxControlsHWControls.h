#ifndef _IntersonCxxControlsHWControls_h
#define _IntersonCxxControlsHWControls_h

#include "IntersonCxx_Export.h"

#include <string>
#include <vector>

namespace IntersonCxx
{

namespace Controls
{

class HWControlsImpl;

class IntersonCxx_EXPORT HWControls
{
public:
  typedef std::vector< std::string > FoundProbesType;
  typedef std::vector< int > FrequenciesType;
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

  void FindAllProbes( FoundProbesType & foundProbes ) const;
  void FindMyProbe( int probeIndex );
  unsigned char GetProbeID() const;

  int ValidDepth( int depth ) const;

  /** Get the list of supported frequecies. */
  void GetFrequency( FrequenciesType & frequencies ) const;

  bool SetFrequency( int frequency );



private:
  HWControls( const HWControls & );
  void operator=( const HWControls & );

  HWControlsImpl * Impl;
};

} // end namespace Controls

} // end namespace IntersonCxx


#endif // _IntersonCxxControlsHWControls_h
