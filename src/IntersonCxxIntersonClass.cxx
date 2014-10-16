// C++
#pragma unmanaged
#include "IntersonCxxIntersonClass.h"

// C#
#pragma managed

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

// Add library from SDK
#using "Interson.dll"

namespace IntersonCxx
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
        //This method converts data between native and managed environments. http://msdn.microsoft.com/en-us/library/bb384859.aspx
        std::string marshalled = msclr::interop::marshal_as< std::string >(Interson::IntersonClass::Version );
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

} // end namespace IntersonCxx
