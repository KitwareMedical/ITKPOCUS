#pragma unmanaged
#include "IntersonCxxControlsHWControls.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"

namespace IntersonCxx
{

namespace Controls
{

class HWControlsImpl
{
public:
  typedef HWControls::FoundProbesType FoundProbesType;
  typedef HWControls::FrequenciesType FrequenciesType;


  HWControlsImpl()
    {
    Wrapped = gcnew Interson::Controls::HWControls();
    }

  unsigned char GetProbeID()
    {
    return Wrapped->GetProbeID();
    }

  void FindAllProbes( FoundProbesType & foundProbes )
    {
    System::Collections::Specialized::StringCollection ^ managedProbes =
      gcnew System::Collections::Specialized::StringCollection();
    Wrapped->FindAllProbes( managedProbes );;
    foundProbes.resize( managedProbes->Count );
    for( int ii = 0; ii < managedProbes->Count; ++ii )
      {
      foundProbes[ii] = msclr::interop::marshal_as< std::string >(
      managedProbes[ii] );
      }
    }

  void FindMyProbe( int probeIndex )
    {
    Wrapped->FindMyProbe( probeIndex );
    }

  int ValidDepth( int depth )
    {
    return Wrapped->ValidDepth( depth );
    }

  void GetFrequency( FrequenciesType & frequencies )
    {
    array< int > ^ managedFrequencies = Wrapped->GetFrequency();
    frequencies.resize( managedFrequencies->Length );
    for( int ii = 0; ii < managedFrequencies->Length; ++ii )
      {
      frequencies[ii] = managedFrequencies[ii];
      }
    }

  bool SetFrequency( int frequency )
    {
    array< int > ^ managedFrequencies = Wrapped->GetFrequency();
    int frequencyIndex = -1;
    for( int ii = 0; ii < managedFrequencies->Length; ++ii )
      {
      if( frequency == managedFrequencies[ii] )
        {
        frequencyIndex = ii;
        break;
        }
      }
    if( frequencyIndex == -1 )
      {
      return false;
      }
    return Wrapped->SetFrequency( frequencyIndex, managedFrequencies[frequencyIndex] );
    }

private:
  gcroot< Interson::Controls::HWControls ^ > Wrapped;
};


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


unsigned char
HWControls
::GetProbeID() const
{
  return Impl->GetProbeID();
}


void
HWControls
::FindAllProbes( FoundProbesType & foundProbes ) const
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
::GetFrequency( FrequenciesType & frequencies ) const
{
  Impl->GetFrequency( frequencies );
}


bool
HWControls
::SetFrequency( int frequency )
{
  return Impl->SetFrequency( frequency );
}

} // end namespace Controls

} // end namespace IntersonCxx
