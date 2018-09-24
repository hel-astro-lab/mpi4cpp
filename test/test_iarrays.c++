#include <mpi2pp/mpi.h>
#include <iostream>

#include <cassert>
#include <array>
#include <vector>

namespace mpi = mpi2pp::mpi;

using requests = std::vector<mpi::request>;


#define NX 1000

// C-style arrays
template<typename T>
bool test_carray(mpi::communicator& world)
{
  mpi::request reqs[2];
  T msg[NX];

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    reqs[0] = world.isend(1, 0, &msg[0], NX);
    reqs[1] = world.irecv(1, 1, &msg[0], NX);
  } else {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    reqs[0] = world.isend(0, 1, &msg[0], NX);
    reqs[1] = world.irecv(0, 0, &msg[0], NX);
  }

  mpi::wait_all(reqs, reqs+2);
  
  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );
  } else {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );
  }

  return true;
}



// std::array
template<typename T>
bool test_array(mpi::communicator& world)
{
  requests reqs(2);
  std::array<T, NX> msg;

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    reqs[0] = world.isend(1, 0, &msg[0], NX);
    reqs[1] = world.irecv(1, 1, &msg[0], NX);
  } else {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    reqs[0] = world.isend(0, 1, &msg[0], NX);
    reqs[1] = world.irecv(0, 0, &msg[0], NX);
  }

  mpi::wait_all(reqs.begin(), reqs.end());
  
  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );
  } else {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );
  }

  return true;
}


// std::vector
template<typename T>
bool test_vector(mpi::communicator& world)
{
  std::vector<T> msg;
  msg.resize(NX);

  requests reqs(2);

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    reqs[0] = world.isend(1, 0, msg);
    reqs[1] = world.irecv(1, 1, msg);
  } else {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    reqs[0] = world.isend(0, 1, msg);
    reqs[1] = world.irecv(0, 0, msg);
  }

  mpi::wait_all(reqs.begin(), reqs.end());
  
  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );
    std::cout << "rank " << world.rank() << " got message " << msg[0] << std::endl;
  } else {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );
    std::cout << "rank " << world.rank() << " got message " << msg[0] << std::endl;
  }

  return true;
}

// std::vector
template<typename T>
bool test_vector_arr(mpi::communicator& world)
{
  requests reqs(2);

  std::vector<T> msg;
  msg.resize(NX);

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(1);
    reqs[0] = world.isend(1, 0, &msg[0], NX);
    reqs[1] = world.irecv(1, 1, &msg[0], NX);
  } else {
    for(int i=0; i<NX; i++) msg[i] = static_cast<T>(2);
    reqs[0] = world.isend(0, 1, &msg[0], NX);
    reqs[1] = world.irecv(0, 0, &msg[0], NX);
  }

  mpi::wait_all(reqs.begin(), reqs.end());
  
  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(2) );
  } else {
    for(int i=0; i<NX; i++) assert(msg[i] == static_cast<T>(1) );
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
