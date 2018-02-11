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
#include "IntersonArrayCxxIntersonClass.h"

// C#

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

// Add library from SDK
#using "IntersonArray.dll"

#pragma managed

namespace IntersonArrayCxx
{
// Wrapper Class for IntersonClass from SDK
class IntersonClassImpl
{
public:
  IntersonClassImpl()
  {
  }

  // Wrapped method for IntersonClass from SDK
  std::string Version()
  {
    // This method converts data between native and managed environments.
    //  http://msdn.microsoft.com/en-us/library/bb384859.aspx
    std::string marshalled = msclr::interop::marshal_as< std::string >(
      IntersonArray::IntersonClass::Version );
    return marshalled;
  }

private:
};


// C++
#pragma unmanaged

IntersonClass::IntersonClass():
  Impl( new IntersonClassImpl() )
{
}

IntersonClass::~IntersonClass()
{
  delete Impl;
}

std::string IntersonClass::Version() const
{
  return Impl->Version();
}

} // end namespace IntersonArrayCxx
