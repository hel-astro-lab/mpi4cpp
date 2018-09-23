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


//--------------------------------------------------
// send/recv no data

void communicator::send(int dest, int tag) const
{
  MPI_CHECK_RESULT(MPI_Send,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          dest, tag, MPI_Comm(*this)));
}

status communicator::recv(int source, int tag) const
{
  status stat;
  MPI_CHECK_RESULT(MPI_Recv,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          source, tag, MPI_Comm(*this), &stat.m_status));
  return stat;
}


} } // ns mpi2pp::mpi
