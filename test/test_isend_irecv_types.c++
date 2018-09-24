#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi2pp::mpi;

template<typename T>
bool test_sending(mpi::communicator& world)
{

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

  reqs[0].wait();
  reqs[1].wait();

  if (world.rank() == 0) {
    assert(msg == 2);
  } else {
    assert(msg == 1);
  }

  
  return true;
}


int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  test_sending<short>(world);
  test_sending<int>(world);
  test_sending<long>(world);
  test_sending<float>(world);
  test_sending<double>(world);
  test_sending<long double>(world);
  test_sending<unsigned short>(world);
  test_sending<unsigned>(world);
  test_sending<unsigned long>(world);

  std::cout << "success!\n";

  return 0;
}
