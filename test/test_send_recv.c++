#include <mpi4cpp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi4cpp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  if (world.rank() == 0) {
    int number_one = 1;
    world.send(1, 0, number_one);

    int msg;
    world.recv(1, 1, msg);
    std::cout << msg << "!" << std::endl;

    assert(msg == 2);

  } else {
    int msg;
    world.recv(0, 0, msg);
    std::cout << msg << ", ";
    std::cout.flush();

    assert(msg == 1);

    int number_two=2;
    world.send(0, 1, number_two);
  }


  std::cout << "success!\n";

  return 0;
}
