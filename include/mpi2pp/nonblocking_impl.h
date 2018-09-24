#include "request.h"

namespace mpi2pp { namespace mpi {


request communicator::isend(int dest, int tag) const
{
  request req;

  MPI_CHECK_RESULT(MPI_Isend,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          dest, tag, MPI_Comm(*this), req.trivial() ));
  return req;
}


request communicator::irecv(int source, int tag) const
{
  request req;
  MPI_CHECK_RESULT(MPI_Irecv,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          source, tag, MPI_Comm(*this), req.trivial() ));
  return req;
}




} } // ns mpi2pp::mpi
