#pragma once

#include <exception>
#include <iostream>
#include <cstdio>


/**
 * Call the MPI routine MPIFunc with arguments Args (surrounded by
 * parentheses). If the result is not MPI_SUCCESS, use assert to abort
 */
#define MPI_CHECK_RESULT( MPIFunc, Args )                               \
 {                                                                      \
   int _check_result = MPIFunc Args;                                    \
   if(_check_result != MPI_SUCCESS) {                                   \
     char _estring[MPI_MAX_ERROR_STRING];                               \
     int _len, _eclass;                                                 \
     MPI_Error_class(_check_result, &_eclass);                          \
     MPI_Error_string(_check_result, _estring, &_len);                  \
     printf("MPI Error %d: %s\n", _eclass, _estring);fflush(stdout);    \
   }                                                                    \
   assert(_check_result == MPI_SUCCESS);                                \
 }


namespace mpi4cpp { namespace mpi {

// Base error class
class MPIerror : public std::exception
{
  public:
  MPIerror() = default;
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


class MPI_Waitany_Error : MPIerror
{
  public:
  MPI_Waitany_Error()
  {
    std::cerr << "MPI_Waitany: XXX" << "\n";
  }
};




} } // ns mpi4cpp::mpi
