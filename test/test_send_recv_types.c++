#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>

namespace mpi = mpi2pp::mpi;

template<typename T>
bool test_sending(mpi::communicator& world)
{
  if (world.rank() == 0) {
    auto msg = static_cast<T>(1);
    world.send(1, 0, msg);

    world.recv(1, 1, msg);
    assert(msg == static_cast<T>(2) );
  } else {

    T msg;
    world.recv(0, 0, msg);

    assert(msg == static_cast<T>(1) );

    msg = static_cast<T>(2);
    world.send(0, 1, msg);
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
