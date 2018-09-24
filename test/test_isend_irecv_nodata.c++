#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi2pp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;
  mpi::request reqs[2];

  if (world.rank() == 0) {
    reqs[0] = world.isend(1, 0);
    reqs[1] = world.irecv(1, 1);
    std::cout << "received tag 1!" << std::endl;

  } else {
    reqs[0] = world.isend(0, 1);
    reqs[1] = world.irecv(0, 0);
  }

  reqs[0].wait();
  reqs[1].wait();

  std::cout << "success!\n";

  return 0;
}
