#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi2pp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  if (world.rank() == 0) {
    world.send(1, 0);
    world.recv(1, 1);
    std::cout << "received tag 1!" << std::endl;

  } else {
    world.recv(0, 0);
    std::cout << "recevied tag 0 ";
    std::cout.flush();

    world.send(0, 1);
  }


  std::cout << "success!\n";

  return 0;
}
