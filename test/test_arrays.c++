#include <mpi4cpp/mpi.h>
#include <iostream>

#include <cassert>
#include <array>
#include <vector>

namespace mpi = mpi4cpp::mpi;

#define NX 100

// C-style arrays
template<typename T>
bool test_carray(mpi::communicator& world)
{

  T msg[NX];

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    world.send(1, 0, &msg[0], NX);

    world.recv(1, 1, &msg[0], NX);
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );

  } else {
    world.recv(0, 0, &msg[0], NX);

    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );

    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    world.send(0, 1, &msg[0], NX);
  }

  return true;
}



// std::array
template<typename T>
bool test_array(mpi::communicator& world)
{

  std::array<T, NX> msg;

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    world.send(1, 0, &msg[0], NX);

    world.recv(1, 1, &msg[0], NX);
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );

  } else {
    world.recv(0, 0, &msg[0], NX);

    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );

    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    world.send(0, 1, &msg[0], NX);
  }

  return true;
}


// std::vector
template<typename T>
bool test_vector(mpi::communicator& world)
{

  std::vector<T> msg;
  msg.resize(NX);

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    world.send(1, 0, msg);

    world.recv(1, 1, msg);
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );

  } else {
    world.recv(0, 0, msg);

    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );

    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    world.send(0, 1, msg);
  }

  return true;
}

// std::vector
template<typename T>
bool test_vector_arr(mpi::communicator& world)
{

  std::vector<T> msg;
  msg.resize(NX);

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    world.send(1, 0, &msg[0], NX);

    world.recv(1, 1, &msg[0], NX);
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );

  } else {
    world.recv(0, 0, &msg[0], NX);

    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );

    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    world.send(0, 1, &msg[0], NX);
  }

  return true;
}

template<typename T>
bool test_all_arrays(mpi::communicator& world)
{
  bool f1 = test_carray<T>(world);
  bool f2 = test_array<T> (world);
  bool f3 = test_vector<T>(world);
  bool f4 = test_vector_arr<T>(world);

  return f1 && f2 && f3 && f4;
}


int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  bool fs[9];

  fs[0] = test_all_arrays<short>(world);
  fs[1] = test_all_arrays<int>(world);
  fs[2] = test_all_arrays<long>(world);
  fs[3] = test_all_arrays<float>(world);
  fs[4] = test_all_arrays<double>(world);
  fs[5] = test_all_arrays<long double>(world);
  fs[6] = test_all_arrays<unsigned short>(world);
  fs[7] = test_all_arrays<unsigned>(world);
  fs[8] = test_all_arrays<unsigned long>(world);


  std::cout << "success!\n";

  return 0;
}
