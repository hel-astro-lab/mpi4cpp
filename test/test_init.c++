#include <mpi4cpp/mpi.h>
#include <iostream>

namespace mpi = mpi4cpp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  std::cout << "success!\n";

  return 0;
}
