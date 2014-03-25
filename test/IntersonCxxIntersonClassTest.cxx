#include "IntersonCxxIntersonClass.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char * argv[] )
{
  IntersonCxx::IntersonClass intersonClass;

  std::cout << "Version: " << intersonClass.Version() << std::endl;
  return EXIT_SUCCESS;
}
