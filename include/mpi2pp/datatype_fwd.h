#pragma once

namespace mpi2pp { namespace mpi {

template<typename T> struct is_mpi_builtin_datatype;
template<typename T> struct is_mpi_integer_datatype;
template<typename T> struct is_mpi_floating_point_datatype;
template<typename T> struct is_mpi_logical_datatype;
template<typename T> struct is_mpi_complex_datatype;
template<typename T> struct is_mpi_byte_datatype;
template<typename T> struct is_mpi_datatype;
template<typename T> MPI_Datatype get_mpi_datatype(const T& x);
template<typename T> MPI_Datatype get_mpi_datatype() 
                                  { return get_mpi_datatype(T());}

/// a dummy data type giving MPI_PACKED as its MPI_Datatype
struct packed {};

} } // end namespace mpi2pp::mpi
