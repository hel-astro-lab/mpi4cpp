#include <mpi4cpp/mpi.h>
#include <iostream>

#include <cassert>
#include <array>
#include <vector>

namespace mpi = mpi4cpp::mpi;

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
  std::vector<T> smsg;
  smsg.resize(NX);

  std::vector<T> rmsg;
  rmsg.resize(NX);

  for(int i=0; i<NX; i++) rmsg[i] = static_cast<T>(3); // indicate error with 3

  requests reqs(2);

  if (world.rank() == 0) {
    for(int i=0; i<NX; i++) smsg[i] = static_cast<T>(1);

    reqs[0] = world.isend(1, 0, smsg);
    reqs[1] = world.irecv(1, 1, rmsg);
  } else {
    for(int i=0; i<NX; i++) smsg[i] = static_cast<T>(2);
    reqs[0] = world.isend(0, 1, smsg);
    reqs[1] = world.irecv(0, 0, rmsg);
  }

  mpi::wait_all(reqs.begin(), reqs.end());
  
  if (world.rank() == 0) {

    for(int i=0; i<NX; i++) 
        if(!(rmsg[i] == static_cast<T>(2))) std::cout << "Got erraneous value for rank 0 at :" << i << " : " << rmsg[i] << " vs " << static_cast<T>(2) << "\n";

    for(int i=0; i<NX; i++) assert(rmsg[i] == static_cast<T>(2) );
    //std::cout << "rank " << world.rank() << " got message " << rmsg[0] << std::endl;
  } else {

    for(int i=0; i<NX; i++) 
        if(!(rmsg[i] == static_cast<T>(1))) std::cout << "Got erraneous value for rank 1 at :" << i << " : " << rmsg[i] << " vs " << static_cast<T>(1) << "\n";


    for(int i=0; i<NX; i++) assert(rmsg[i] == static_cast<T>(1) );
    //std::cout << "rank " << world.rank() << " got message " << msg[0] << std::endl;
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

  test_all_arrays<short>(world);
  test_all_arrays<int>(world);
  test_all_arrays<long>(world);
  test_all_arrays<float>(world);
  test_all_arrays<double>(world);
  test_all_arrays<long double>(world);
  test_all_arrays<unsigned short>(world);
  test_all_arrays<unsigned>(world);
  test_all_arrays<unsigned long>(world);


  std::cout << "success!\n";

  return 0;
}
