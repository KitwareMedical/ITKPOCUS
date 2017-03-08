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
// C++
#pragma unmanaged
#include "IntersonArrayCxxControlsHWControls.h"

// C#
#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

// Add library from SDK
#using "IntersonArray.dll"

namespace IntersonArrayCxx
{

namespace Controls
{

ref class NewHardButtonHandler
{
public:
  typedef HWControls::NewHardButtonCallbackType   NewHardButtonCallbackType;

  NewHardButtonHandler():
    NewHardButtonCallback( NULL ),
    NewHardButtonCallbackClientData( NULL )
  {
  }

  ~NewHardButtonHandler()
  {
  }

  void HandleNewHardButton( IntersonArray::Controls::HWControls ^controls,
    System::EventArgs ^eventArgs )
  {
    if ( this->NewHardButtonCallback != NULL )
    {
      this->NewHardButtonCallback( this->NewHardButtonCallbackClientData );
    }
  }

  void SetNewHardButtonCallback( NewHardButtonCallbackType callback,
    void *clientData )
  {
    this->NewHardButtonCallback = callback;
    this->NewHardButtonCallbackClientData = clientData;
  }

private:
  NewHardButtonCallbackType NewHardButtonCallback;
  void *NewHardButtonCallbackClientData;
};

// Wrapper Class for IntersonArrayClass from SDK
class HWControlsImpl
{
public:
  // New defininions look header file
  typedef HWControls::FoundProbesType FoundProbesType;
  typedef HWControls::FrequenciesType FrequenciesType;
  typedef HWControls::FocusType       FocusType;


  HWControlsImpl()
  {
    // New method for managed code. Wrapped is data member or this class
    Wrapped = gcnew IntersonArray::Controls::HWControls();

    HardButtonHandler = gcnew NewHardButtonHandler();
    HardButtonHandlerDelegate = gcnew
      IntersonArray::Controls::HWControls::HWButtonHandler(
        HardButtonHandler, &NewHardButtonHandler::HandleNewHardButton );
    Wrapped->HWButtonTick += HardButtonHandlerDelegate;
  }

  ~HWControlsImpl()
  {
  Wrapped->HWButtonTick -= HardButtonHandlerDelegate;
  }

  // Compound Angle Functions
  int GetCompoundAngle() const
  {
    return Wrapped->CompoundAngle;
  }

  bool EnableCompound()
  {
    return Wrapped->EnableCompound();
  }

  bool DisableCompound()
  {
    return Wrapped->DisableCompound();
  }

  // Doubler Functions
  bool EnableDoubler()
  {
    return Wrapped->EnableDoubler();
  }

  bool DisableDoubler()
  {
    return Wrapped->DisableDoubler();
  }


  // Probe ID Functions
  short GetProbeID() const
  {
    return Wrapped->GetProbeID();
  }

  void FindAllProbes( FoundProbesType &foundProbes ) const
  {
    System::Collections::Specialized::StringCollection ^managedProbes =
      gcnew System::Collections::Specialized::StringCollection();
    Wrapped->FindAllProbes( managedProbes );;
    foundProbes.resize( managedProbes->Count );
    // Copy all available probes to the C++ class
    for ( int ii = 0; ii < managedProbes->Count; ++ii )
    {
      foundProbes[ii] = msclr::interop::marshal_as< std::string >(
                          managedProbes[ii] );
    }
  }

  void FindMyProbe( int probeIndex ) const
  {
    Wrapped->FindMyProbe( probeIndex );
  }

  // Angle
  float GetArrayAngle() const
  {
    return Wrapped->GetArrayAngle();
  }

  // Radius
  float GetArrayRadius() const
  {
    return Wrapped->GetArrayRadius();
  }

  // Width
  float GetArrayWidth() const
  {
    return Wrapped->GetArrayWidth();
  }

  // Focus
  void GetFocus( FocusType & focus ) const
  {
    array< int > ^managedFocus = Wrapped->GetFocus();
    focus.resize( managedFocus->Length );
    for ( int ii = 0; ii < managedFocus->Length; ++ii )
    {
      focus[ii] = managedFocus[ii];
    }
  }

  // To check if the chosen Depth is valid for the probe. Returns the same
  // depth if in the max range, otherwise returns the max depth
  int ValidDepth( int depth ) const
  {
    return Wrapped->ValidDepth( depth );
  }

  // Get the list of supported frequencies. Units: Hz.
  void GetFrequency( FrequenciesType &frequencies ) const
  {
    array< int > ^managedFrequencies = Wrapped->GetFrequency();
    frequencies.resize( managedFrequencies->Length );
    for ( int ii = 0; ii < managedFrequencies->Length; ++ii )
    {
      frequencies[ii] = managedFrequencies[ii];
    }
  }

  // Set the list of supported frequencies. Units: Hz.
  bool SetFrequencyAndFocus( unsigned char frequency, unsigned char focus,
    int steering )
  {
    return Wrapped->SetFrequencyAndFocus( frequency, focus, steering );
  }

  // Lines Per Array
  unsigned int GetLinesPerArray() const
  {
    return Wrapped->GetLinesPerArray();
  }

  //" voltage ": percentage of the maximum of the High Voltage. Returns
  //whether the command succeeded (true) or failed (false)
  bool SendHighVoltage( unsigned char voltage, unsigned char voltageCfm )
  {
    return Wrapped->SendHighVoltage( voltage, voltageCfm );
  }

  bool EnableHighVoltage()
  {
    return Wrapped->EnableHighVoltage();
  }

  bool DisableHighVoltage()
  {
    return Wrapped->DisableHighVoltage();
  }

  // To set the value of the dynamic and to calculate the value sent to
  // the control. " bytDynamic ": value of the Dynamic (db).
  // Returns: Returns whether the command succeeded (true) or failed (false)
  bool SendDynamic( unsigned char dynamic )
  {
    return Wrapped->SendDynamic( dynamic );
  }

  // To Start watching the Hardware Button to enable to raise the event
  // HWButtonTick
  void EnableHardButton()
  {
    Wrapped->EnableHardButton();
  }

  void DisableHardButton()
  {
    Wrapped->DisableHardButton();
  }

  unsigned char ReadHardButton() const
  {
    return Wrapped->ReadHardButton();
  }

  void SetNewHardButtonCallback( HWControls::NewHardButtonCallbackType
    callback, void *clientData = 0 )
  {
    this->HardButtonHandler->SetNewHardButtonCallback( callback,
      clientData );
  }

  bool StartBmode()
  {
    return Wrapped->StartBmode();
  }

  bool StartRFmode()
  {
    return Wrapped->StartRFmode();
  }

  bool StopAcquisition()
  {
    return Wrapped->StopAcquisition();
  }

  short GetProbeFrameRate() const
  {
    return Wrapped->GetProbeFrameRate();
  }

  std::string GetProbeSerialNumber() const
  {
    return msclr::interop::marshal_as< std::string >(
      Wrapped->GetProbeSerialNumber() );
  }

  std::string ReadFPGAVersion() const
  {
    return msclr::interop::marshal_as< std::string >(
      Wrapped->ReadFPGAVersion() );
  }

  std::string GetOEMId() const
  {
    return msclr::interop::marshal_as< std::string >(
      IntersonArray::Controls::HWControls::GetOEMId() );
  }

  std::string Get3DId() const
  {
    return msclr::interop::marshal_as< std::string >(
      IntersonArray::Controls::HWControls::Get3DId() );
  }

  std::string GetFilterId()
  {
    return msclr::interop::marshal_as< std::string >(
      IntersonArray::Controls::HWControls::GetFilterId() );
  }


private:

  gcroot< IntersonArray::Controls::HWControls ^ > Wrapped;
  gcroot< NewHardButtonHandler ^ >                HardButtonHandler;
  gcroot< IntersonArray::Controls::HWControls::HWButtonHandler ^ >
                                                  HardButtonHandlerDelegate;

};

// C++
#pragma unmanaged

HWControls
::HWControls():
  Impl( new HWControlsImpl() )
{
}


HWControls
::~HWControls()
{
  delete Impl;
}


short
HWControls
::GetProbeID() const
{
  return Impl->GetProbeID();
}


void
HWControls
::FindAllProbes( FoundProbesType &foundProbes ) const
{
  Impl->FindAllProbes( foundProbes );
}


void
HWControls
::FindMyProbe( int probeIndex )
{
  Impl->FindMyProbe( probeIndex );
}

int
HWControls
::ValidDepth( int depth ) const
{
  return Impl->ValidDepth( depth );
}


void
HWControls
::GetFrequency( FrequenciesType &frequencies ) const
{
  Impl->GetFrequency( frequencies );
}


void
HWControls
::GetFocus( FocusType &focus ) const
{
  Impl->GetFocus( focus );
}


bool
HWControls
::SetFrequencyAndFocus( unsigned char frequency, unsigned char focus,
  int steering )
{
  return Impl->SetFrequencyAndFocus( frequency, focus, steering );
}


bool
HWControls
::SendHighVoltage( unsigned char voltage, unsigned char voltageCfm )
{
  return Impl->SendHighVoltage( voltage, voltageCfm );
}


bool
HWControls
::EnableHighVoltage()
{
  return Impl->EnableHighVoltage();
}


bool
HWControls
::DisableHighVoltage()
{
  return Impl->DisableHighVoltage();
}


bool
HWControls
::SendDynamic( unsigned char dynamic )
{
  return Impl->SendDynamic( dynamic );
}


void
HWControls
::EnableHardButton()
{
  Impl->EnableHardButton();
}


void
HWControls
::DisableHardButton()
{
  Impl->DisableHardButton();
}


unsigned char
HWControls
::ReadHardButton()
{
  return Impl->ReadHardButton();
}


bool
HWControls
::StartBmode()
{
  return Impl->StartBmode();
}


bool
HWControls
::StartRFmode()
{
  return Impl->StartRFmode();
}


bool
HWControls
::StopAcquisition()
{
  return Impl->StopAcquisition();
}


short
HWControls
::GetProbeFrameRate() const
{
  return Impl->GetProbeFrameRate();
}


std::string
HWControls
::GetProbeSerialNumber() const
{
  return Impl->GetProbeSerialNumber();
}


std::string
HWControls
::ReadFPGAVersion() const
{
  return Impl->ReadFPGAVersion();
}


std::string
HWControls
::GetOEMId() const
{
  return Impl->GetOEMId();
}


std::string
HWControls
::GetFilterId() const
{
  return Impl->GetFilterId();
}

} // end namespace Controls

} // end namespace IntersonArrayCxx
