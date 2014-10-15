#ifndef _IntersonCxxControlsHWControls_h
#define _IntersonCxxControlsHWControls_h

// Export library, autogenerated using Cmake
#include "IntersonCxx_Export.h"

#include <string>
#include <vector>

// Namespace similar to Interson Namespace from SDK

/*
The library is organized via namespaces; the root is �Interson�,
then �Interson.Controls� to get the methods which access to the 
USB probe controls, �Interson.Imaging� is the scan converter, to 
build the 2D image B-Scan bitmap from the log-compressed A-Scan 
vectors.
*/
namespace IntersonCxx
{
	// Namespace similar to Interson.Controls Namespace from SDK
	namespace Controls
	{
		// Wrapper Class for IntersonClass from SDK
		class HWControlsImpl;

		// HWControls similar to IntersonClass from SDK
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

			  // Maximum number of frequencies
			  static const int MAX_FREQ = 3;

			  // These are the valid Probe IDs.
			  static const int ID_GP_3_5MHz = 1;
			  static const int ID_VC_7_5MHz = 2;
			  static const int ID_SP_7_5MHz = 3;
			  static const int ID_EC_7_5MHz = 4;
			  static const int ID_MV_12_MHz = 5;
			  static const int ID_GV_5_MHz = 6;

			  // Size of the OEM (Original equipment manufacturer) space in the Probe EEPROM number of frequencies. http://en.wikipedia.org/wiki/Original_equipment_manufacturer
			  static const unsigned short int EEOEMSpace = 4096;

			  // String Vector with all probe names connected
			  void FindAllProbes( FoundProbesType & foundProbes ) const;
			  // Summary: To open a handle to the probe defined by the ProbeIndex. Parameters:	" ProbeIndex ": Index in the mycolProbesName collection
			  void FindMyProbe( int probeIndex );
			  //To get the ID of the probe
			  unsigned char GetProbeID() const;

			  /** Units: mm */
			  // To check if the chosen Depth is valid for the probe. Returns the same depth if in the max range, otherwise returns the max depth
			  int ValidDepth( int depth ) const;

			  /** Get the list of supported frequencies. Units: Hz. */
			  void GetFrequency( FrequenciesType & frequencies ) const;
			  bool SetFrequency( int frequency );

			  /** Units: Volts */
			  //" voltage ": percentage of the maximum of the High Voltage. Returns whether the command succeeded (true) or failed (false)
			  bool SendHighVoltage( unsigned char voltage );
			  bool EnableHighVoltage();
			  bool DisableHighVoltage();

			  // To set the value of the dynamic and to calculate the value sent to the control. " bytDynamic ": value of the Dynamic (db). 
			  // Returns: Returns whether the command succeeded (true) or failed (false)
			  bool SendDynamic( unsigned char dynamic );

			  //To start/stop the motor of the probe
			  bool StartMotor();
			  bool StopMotor();

			  // To Start watching the Hardware Button to enable to raise the event �HWButtonTick�
			  void EnableHardButton();
			  void DisableHardButton();
			  unsigned char ReadHardButton();

			  bool StartBmode();
			  bool StartRFmode();
			  bool StopAcquisition();

			  //To get the Frame Rate of that probe according to the depth
			  //" iDepth ": current depth. Returns: Return the current Frame Rate
			  short GetProbeFrameRate( int depth );
			  
			  // To get the Serial Number of the probe
			  std::string GetProbeSerialNumber() const;

			  // To get the version of the software installed in the FPGA.
			  std::string ReadFPGAVersion() const;

			  // To get the identification of the OEM
			  std::string GetOEMId() const;

			  //To get the identification of the filter.
			  std::string GetFilterId() const;

			  //To enable the RF Decimator. Returns: Returns whether the command succeeded (true) or failed (false)
			  /*
			  Note: For GV and GP (low frequency probes) sampling rate is 15 Msamples/sec with 
			  decimator off and with decimator on sampling rate is 7.5 Msamples/sec. For higher
			  frequency probes sampling rate is 30 Ms/sec with decimator off and 15 Ms/sec with
			  decimator on. With the speed of ultrasound at 1540 m/sec in human tissue: 30 Ms/sec
			  is 0.1 mm/sample, 15 Ms/sec is 0.2 mm/sample, 7.5 Ms/sec is 0.4 mm/sample. Therfore
			  with 2048 samples, the depths are 5.25 cm, 10.5 cm and 21 cm.
			  */
			  bool EnableRFDecimator();
			  bool DisableRFDecimator();

			private:
			  HWControls( const HWControls & );
			  void operator=( const HWControls & );

			  HWControlsImpl * Impl;
		};

	} // end namespace Controls

} // end namespace IntersonCxx


#endif // _IntersonCxxControlsHWControls_h
