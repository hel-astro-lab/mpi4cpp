#include <mpi4cpp/mpi.h>
#include <iostream>

#include <array>
#include <vector>
#include <tuple>


/// user-defined class
class Blob
{
  public:
  int ivar;
  float var;
  std::array<double,3> x;
};

//--------------------------------------------------


// introduce class to mpi
namespace mpi4cpp { namespace mpi {

template <>
struct is_mpi_datatype<Blob> 
  : mpl::true_ { };

template <>
MPI_Datatype get_mpi_datatype<Blob>(const Blob& blob) 
{
    
  // get start of the class
  MPI_Aint base;
  MPI_Get_address( &blob, &base ); 

  //--------------------------------------------------

  // how many elements per each type
  std::array<int, 3> block_lengths{
    { 1, 1, 3 }
  };

  // and then the actual members
  std::array<MPI_Aint, 3> member_offsets; // relative offsets
  MPI_Get_address( &blob.ivar,   &member_offsets[0]);
  MPI_Get_address( &blob.var,    &member_offsets[1]);
  MPI_Get_address( &blob.x[0],   &member_offsets[2]);

  // create real (absolute) offsets (=rel - base)
  std::array<MPI_Aint, 3> offsets {
    member_offsets[0] - base,
    member_offsets[1] - base,
    member_offsets[2] - base
  };

  /*
  std::cout << "offsets are "
    << " 0 " << offsets[0] << " / " << member_offsets[0] << " x " << base 
    << " 1 " << offsets[1] << " / " << member_offsets[1] << " x " << base 
    << " 2 " << offsets[2] << " / " << member_offsets[2] << " x " << base 
    << "\n";
  */

  // introduce datatypes
  std::array<MPI_Datatype, 3> datatypes{
    { MPI_INT, MPI_FLOAT, MPI_DOUBLE }
  };

  //--------------------------------------------------
  // create datatype; this is standard format and should not be changed
  MPI_Datatype blob_type;
  MPI_Type_create_struct(
      block_lengths.size(),
      block_lengths.data(),
      offsets.data(),
      datatypes.data(),
      &blob_type);

  MPI_Type_commit(&blob_type);
  return blob_type;
}

} } // ns mpi4cpp::mpi
//--------------------------------------------------
namespace mpi = mpi4cpp::mpi;


bool test_single_message(mpi::communicator& world)
{

  Blob msg;
  if (world.rank() == 0) {
    msg.ivar = 10;
    msg.var  = 11.0;
    msg.x[0] = 12.0;
    msg.x[1] = 13.0;
    msg.x[2] = 14.0;
    world.send(1, 0, msg);
  } else {
    world.recv(0, 0, msg);
  }

  /*
  std::cout << world.rank() << ": blob is "
    << " ivar:"  << msg.ivar 
    << " var:" << msg.var 
    << " arr0:" << msg.x[0]
    << " arr1:" << msg.x[1]
    << " arr2:" << msg.x[2] 
    << "\n";
  */

  assert( msg.ivar  == 10   );
  assert( msg.var   == 11.0 );
  assert( msg.x[0]  == 12.0 );
  assert( msg.x[1]  == 13.0 );
  assert( msg.x[2]  == 14.0 );

  return true;
}

bool test_nonblocking_single_message(mpi::communicator& world)
{

  mpi::request reqs[2];
  Blob msg;
  if (world.rank() == 0) {
    msg.ivar = 10;
    msg.var  = 11.0;
    msg.x[0] = 12.0;
    msg.x[1] = 13.0;
    msg.x[2] = 14.0;

    reqs[0] = world.isend(1, 0, msg);
  } else {
    reqs[0] = world.irecv(0, 0, msg);
  }
  mpi::wait_all(reqs, reqs+1);

  assert( msg.ivar  == 10   );
  assert( msg.var   == 11.0 );
  assert( msg.x[0]  == 12.0 );
  assert( msg.x[1]  == 13.0 );
  assert( msg.x[2]  == 14.0 );

  return true;
}


bool test_vector_message(mpi::communicator& world)
{

  std::vector<Blob> msgs(10);
  if (world.rank() == 0) {

    // create vector of messages
    for (auto& elem : msgs) {
      elem.ivar = 10;
      elem.var  = 11.0;
      elem.x[0] = 12.0;
      elem.x[1] = 13.0;
      elem.x[2] = 14.0;
    }

    // send vector
    world.send(1, 0, msgs);
  } else {
    world.recv(0, 0, msgs);
  }

  for (auto& elem : msgs) {
    assert( elem.ivar  == 10   );
    assert( elem.var   == 11.0 );
    assert( elem.x[0]  == 12.0 );
    assert( elem.x[1]  == 13.0 );
    assert( elem.x[2]  == 14.0 );
  }

  return true;
}




int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);
  mpi::communicator world;

  bool f1 = test_single_message(world);
  bool f2 = test_vector_message(world);
  bool f3 = test_nonblocking_single_message(world);

  assert(f1);
  assert(f2);
  assert(f3);

  return 0;
}
