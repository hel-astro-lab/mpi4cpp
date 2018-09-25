#include "status.h"


namespace mpi2pp { namespace mpi {

bool status::cancelled() const
{
  int flag = 0;
  MPI_CHECK_RESULT(MPI_Test_cancelled, (&m_status, &flag));
  return flag != 0;
}



} } // ns mpi2pp::mpi
