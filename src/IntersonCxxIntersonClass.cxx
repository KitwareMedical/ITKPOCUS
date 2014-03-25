#pragma unmanaged
#include "IntersonCxxIntersonClass.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"

namespace IntersonCxx
{

class IntersonClassImpl
{
public:
  IntersonClassImpl()
    {
    }

  std::string Version()
    {
    std::string marshalled = msclr::interop::marshal_as< std::string >(
    Interson::IntersonClass::Version );
    return marshalled;
    }

private:
};


#pragma unmanaged

IntersonClass
::IntersonClass():
  Impl( new IntersonClassImpl() )
{
}

IntersonClass
::~IntersonClass()
{
  delete Impl;
}

std::string
IntersonClass
::Version() const
{
  return Impl->Version();
}

} // end namespace IntersonCxx
