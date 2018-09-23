#include <mpi2pp/mpi.h>
#include <iostream>

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

  /*
  if (world.rank() == 0) {
    int number_one = 1;
    world.send(1, 0, number_one);

    int msg;
    world.recv(1, 1, msg);
    std::cout << msg << "!" << std::endl;
  } else {
    int msg;
    world.recv(0, 0, msg);
    std::cout << msg << ", ";
    std::cout.flush();

    int number_two=2;
    world.send(0, 1, number_two);
  }
  */

  std::cout << "success!\n";

  return 0;
}
