#include <mpi2pp/mpi.h>
#include <iostream>

namespace mpi = mpi2pp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  std::cout << "success!\n";

  return 0;
}
