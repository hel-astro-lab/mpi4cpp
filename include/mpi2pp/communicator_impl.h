#include "communicator.h"


namespace mpi2pp { namespace mpi {


communicator::communicator()
{
  comm_ptr.reset(new MPI_Comm(MPI_COMM_WORLD));
}

int communicator::size() const
{
  int size_;
  MPI_CHECK_RESULT(MPI_Comm_size, (MPI_Comm(*this), &size_));
  return size_;
}

int communicator::rank() const
{
  int rank_;
  MPI_CHECK_RESULT(MPI_Comm_rank, (MPI_Comm(*this), &rank_));
  return rank_;
}

communicator::operator MPI_Comm() const
{
  if (comm_ptr) return *comm_ptr;
  else return MPI_COMM_NULL;
}


void communicator::abort(int errcode) const
{
  MPI_CHECK_RESULT(MPI_Abort, (MPI_Comm(*this), errcode));
  std::abort();
}


void (communicator::barrier)() const
{
  MPI_CHECK_RESULT(MPI_Barrier, (MPI_Comm(*this)));
}


} } // ns mpi2pp::mpi
