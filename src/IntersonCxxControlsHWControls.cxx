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
::FindAllProbes( FoundProbesType & foundProbes )
{
  Impl->FindAllProbes( foundProbes );
}


void
HWControls
::FindMyProbe( int probeIndex )
{
  Impl->FindMyProbe( probeIndex );
}

} // end namespace Controls

} // end namespace IntersonCxx
