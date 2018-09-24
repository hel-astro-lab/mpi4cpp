#pragma once

#include <exception>
#include <iostream>

/**
 * Call the MPI routine MPIFunc with arguments Args (surrounded by
 * parentheses). If the result is not MPI_SUCCESS, use assert to abort
 */
#define MPI_CHECK_RESULT( MPIFunc, Args )                               \
 {                                                                      \
   int _check_result = MPIFunc Args;                                    \
   assert(_check_result == MPI_SUCCESS);                                \
 }



namespace mpi2pp { namespace mpi {

// Base error class
class MPIerror : public std::exception
{
  public:
  MPIerror() {};
};

class MPI_Waitall_Error : MPIerror
{
  public:
  //MPI_Waitall_Error(MPI_Error_string& err) 
  //{
  //  std::cerr << "MPI_Waitall: " << err << "\n";
  //}
  MPI_Waitall_Error()
  {
    std::cerr << "MPI_Waitall: XXX" << "\n";
  }
};

class MPI_Testall_Error : MPIerror
{
  public:
  MPI_Testall_Error()
  {
    std::cerr << "MPI_Testall: XXX" << "\n";
  }
};



} } // ns mpi2pp::mpi
