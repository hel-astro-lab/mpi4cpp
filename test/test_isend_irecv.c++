#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi2pp::mpi;

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;
  mpi::request reqs[2];

  int msg;
  if (world.rank() == 0) {
    int number_one = 1;
    reqs[0] = world.isend(1, 0, number_one);
    reqs[1] = world.irecv(1, 1, msg);
  } else {
    int number_two=2;
    reqs[0] = world.isend(0, 1, number_two);
    reqs[1] = world.irecv(0, 0, msg);
  }

  mpi::wait_all(reqs, reqs+2);

  if (world.rank() == 0) {
    assert(msg == 2);
  } else {
    assert(msg == 1);
  }
  

  std::cout << "success!\n";

  return 0;
}
