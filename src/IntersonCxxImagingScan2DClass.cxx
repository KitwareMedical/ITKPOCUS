#pragma unmanaged
#include "IntersonCxxImagingScan2DClass.h"

#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#using "Interson.dll"

namespace IntersonCxx
{

namespace Imaging
{

class Scan2DClassImpl
{
public:
  Scan2DClassImpl()
    {
    Wrapped = gcnew Interson::Imaging::Scan2DClass();
    }

private:
  gcroot< Interson::Imaging::Scan2DClass ^ > Wrapped;
};


#pragma unmanaged

Scan2DClass
::Scan2DClass():
  Impl( new Scan2DClassImpl() )
{
}


Scan2DClass
::~Scan2DClass()
{
  delete Impl;
}

} // end namespace Imaging

} // end namespace IntersonCxx
