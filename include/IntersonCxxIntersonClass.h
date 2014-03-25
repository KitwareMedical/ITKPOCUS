#ifndef _IntersonCxxIntersonClass_h
#define _IntersonCxxIntersonClass_h

#include "IntersonCxx_Export.h"

#include <string>

#pragma unmanaged

namespace IntersonCxx
{

class IntersonClassImpl;

class IntersonCxx_EXPORT IntersonClass
{
public:
  IntersonClass();
  ~IntersonClass();

  //std::string CompleteVersion();

  //std::string Version();

private:
  IntersonClass( const IntersonClass & );
  void operator=( const IntersonClass & );

  IntersonClassImpl * Impl;
};

} // end namespace IntersonCxx

#endif // _IntersonCxxIntersonClass_h
