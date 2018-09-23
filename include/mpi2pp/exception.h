#pragma once



/**
 * Call the MPI routine MPIFunc with arguments Args (surrounded by
 * parentheses). If the result is not MPI_SUCCESS, use assert to abort
 */
#define MPI_CHECK_RESULT( MPIFunc, Args )                               \
 {                                                                      \
   int _check_result = MPIFunc Args;                                    \
   assert(_check_result == MPI_SUCCESS);                                \
 }
