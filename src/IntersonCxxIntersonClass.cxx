#include "IntersonCxxIntersonClass.h"

namespace IntersonCxx
{

#pragma managed

class IntersonClassImpl
{
public:
  IntersonClassImpl()
    {
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

 
} // end namespace IntersonCxx
