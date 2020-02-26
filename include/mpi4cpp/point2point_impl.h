#pragma once

namespace mpi4cpp { namespace mpi {

//--------------------------------------------------
// send/recv no data

inline void 
communicator::send(int dest, int tag) const
{
  MPI_CHECK_RESULT(MPI_Send,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          dest, tag, MPI_Comm(*this)));
}

inline status 
communicator::recv(int source, int tag) const
{
  status stat;
  MPI_CHECK_RESULT(MPI_Recv,
                         (MPI_BOTTOM, 0, MPI_PACKED,
                          source, tag, MPI_Comm(*this), &stat.m_status));
  return stat;
}



//--------------------------------------------------
// flat POD object

// We're sending a type that has an associated MPI datatype, so we
// map directly to that datatype.
template<typename T>
inline void
communicator::send_impl(int dest, int tag, const T& value, mpl::true_ /*unused*/) const
{
  MPI_CHECK_RESULT(MPI_Send,
                  (const_cast<T*>(&value), 1, get_mpi_datatype<T>(value),
                  dest, tag, MPI_Comm(*this)));
}

// We're receiving a type that has an associated MPI datatype, so we
// map directly to that datatype.
template<typename T>
inline status 
communicator::recv_impl(int source, int tag, T& value, mpl::true_ /*unused*/) const
{
  status stat;

  MPI_CHECK_RESULT(MPI_Recv,
                  (const_cast<T*>(&value), 1, 
                  get_mpi_datatype<T>(value),
                  source, tag, MPI_Comm(*this), &stat.m_status));
  return stat;
}

//--------------------------------------------------

// Single-element receive may either send the element directly or
// serialize it via a buffer.
template<typename T>
inline void 
communicator::send(int dest, int tag, const T& value) const
{
  this->send_impl(dest, tag, value, is_mpi_datatype<T>());
}

// Single-element receive may either receive the element directly or
// deserialize it from a buffer.
template<typename T>
inline status 
communicator::recv(int source, int tag, T& value) const
{
  return this->recv_impl(source, tag, value, is_mpi_datatype<T>());
}

//--------------------------------------------------

// We're sending an array of a type that has an associated MPI
// datatype, so we map directly to that datatype.
template<typename T>
inline void
communicator::array_send_impl(int dest, int tag, const T* values, int n,
                              mpl::true_ /*unused*/) const
{
  MPI_CHECK_RESULT(MPI_Send,
                  (const_cast<T*>(values), n, 
                  get_mpi_datatype<T>(*values),
                  dest, tag, MPI_Comm(*this)));
}

template<typename T>
inline status 
communicator::array_recv_impl(int source, int tag, T* values, int n, 
                              mpl::true_ /*unused*/) const
{
  status stat;
  MPI_CHECK_RESULT(MPI_Recv,
                  (const_cast<T*>(values), n, 
                  get_mpi_datatype<T>(*values),
                  source, tag, MPI_Comm(*this), &stat.m_status));
  return stat;
}

//--------------------------------------------------

// vector of a type has an associated MPI datatype, so we map directly to 
// that datatype
template<typename T, typename A>
inline void 
communicator::send_vector(int dest, int tag, 
  const std::vector<T,A>& value, mpl::true_ true_type) const
{
  // send the vector size
  typename std::vector<T,A>::size_type size = value.size();
  send(dest, tag, size);
  // send the data
  this->array_send_impl(dest, tag, value.data(), size, true_type);
}

template<typename T, typename A>
inline status 
communicator::recv_vector(int source, int tag, 
  std::vector<T,A>& value, mpl::true_ true_type) const
{
  // receive the vector size
  typename std::vector<T,A>::size_type size = 0;
  recv(source, tag, size);
  // size the vector
  value.resize(size);
  // receive the data
  return this->array_recv_impl(source, tag, value.data(), size, true_type);
}

//--------------------------------------------------

template<typename T, typename A>
inline void 
communicator::send(int dest, int tag, const std::vector<T,A>& value) const
{
  send_vector(dest, tag, value, is_mpi_datatype<T>());
}

template<typename T, typename A>
inline status 
communicator::recv(int source, int tag, std::vector<T,A>& value) const
{
  return recv_vector(source, tag, value, is_mpi_datatype<T>());
}

//--------------------------------------------------

// Array send must send the elements directly
template<typename T>
inline void 
communicator::send(int dest, int tag, const T* values, int n) const
{
  this->array_send_impl(dest, tag, values, n, is_mpi_datatype<T>());
}

// Array receive must receive the elements directly into a buffer.
template<typename T>
inline status 
communicator::recv(int source, int tag, T* values, int n) const
{
  return this->array_recv_impl(source, tag, values, n, is_mpi_datatype<T>());
}

//--------------------------------------------------


} } // ns mpi4cpp::mpi
