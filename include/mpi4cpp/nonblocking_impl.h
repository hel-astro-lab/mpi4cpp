#include "request.h"

namespace mpi4cpp { namespace mpi {

//--------------------------------------------------

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

//--------------------------------------------------
// send value

// We're sending a type that has an associated MPI datatype, so we
// map directly to that datatype.
template<typename T>
request
communicator::isend_impl(int dest, int tag, const T& value, mpl::true_) const
{
  request req;
  MPI_CHECK_RESULT(MPI_Isend,
                         (const_cast<T*>(&value), 1, 
                          get_mpi_datatype<T>(value),
                          dest, tag, MPI_Comm(*this), req.trivial()));
  return req;
}

// Single-element receive may either send the element directly or
// serialize it via a buffer.
template<typename T>
request communicator::isend(int dest, int tag, const T& value) const
{
  return this->isend_impl(dest, tag, value, is_mpi_datatype<T>());
}


// We're receiving a type that has an associated MPI datatype, so we
// map directly to that datatype.
template<typename T>
request 
communicator::irecv_impl(int source, int tag, T& value, mpl::true_) const
{
  request req;
  MPI_CHECK_RESULT(MPI_Irecv,
                         (const_cast<T*>(&value), 1, 
                          get_mpi_datatype<T>(value),
                          source, tag, MPI_Comm(*this), req.trivial()));
  return req;
}

template<typename T>
request 
communicator::irecv(int source, int tag, T& value) const
{
  return this->irecv_impl(source, tag, value, is_mpi_datatype<T>());
}



//--------------------------------------------------
// send/recv vector
  
template<typename T, class A>
request communicator::isend(int dest, int tag, const std::vector<T,A>& values) const
{
  return this->isend_vector(dest, tag, values, is_mpi_datatype<T>());
}

template<typename T, class A>
request
communicator::isend_vector(int dest, int tag, const std::vector<T,A>& values,
                           mpl::true_) const
{
  std::size_t size = values.size();
  request req = this->isend_impl(dest, tag, size, mpl::true_());
  MPI_CHECK_RESULT(MPI_Isend,
                         (const_cast<T*>(values.data()), size, 
                          get_mpi_datatype<T>(),
                          dest, tag, MPI_Comm(*this), &req.payload_request()));
  return req;
  
}

namespace detail {
  /**
   * Internal data structure that stores everything required to manage
   * the receipt of an array of primitive data but unknown size.
   * Such an array can have been send with blocking operation and so must
   * be compatible with the (size_t,raw_data[]) format.
   */
  template<typename T, class A>
  struct dynamic_array_irecv_data
  {
    //BOOST_STATIC_ASSERT_MSG(is_mpi_datatype<T>::value, "Can only be specialized for MPI datatypes.");

    dynamic_array_irecv_data(const communicator& comm, int source, int tag, 
                             std::vector<T,A>& values)
      : comm(comm), source(source), tag(tag), count(-1), values(values)
    { 
    }

    communicator comm;
    int source;
    int tag;
    std::size_t count;
    std::vector<T,A>& values;
  };

}

template<typename T, class A>
std::optional<status> 
request::handle_dynamic_primitive_array_irecv(request* self, request_action action)
{
  typedef detail::dynamic_array_irecv_data<T,A> data_t;
  std::shared_ptr<data_t> data = std::static_pointer_cast<data_t>(self->m_data);

  if (action == ra_wait) {
    status stat;
    if (self->m_requests[1] == MPI_REQUEST_NULL) {
      // Wait for the count message to complete
      MPI_CHECK_RESULT(MPI_Wait, (self->m_requests, &stat.m_status));
      // Resize our buffer and get ready to receive its data
      data->values.resize(data->count);
      MPI_CHECK_RESULT(MPI_Irecv,
                             (&(data->values[0]), data->values.size(), get_mpi_datatype<T>(),
                              stat.source(), stat.tag(), 
                              MPI_Comm(data->comm), self->m_requests + 1));
    }
    // Wait until we have received the entire message
    MPI_CHECK_RESULT(MPI_Wait, (self->m_requests + 1, &stat.m_status));
    return stat;
  } else if (action == ra_test) {
    status stat;
    int flag = 0;

    if (self->m_requests[1] == MPI_REQUEST_NULL) {
      // Check if the count message has completed
      MPI_CHECK_RESULT(MPI_Test, (self->m_requests, &flag, &stat.m_status));
      if (flag) {
        // Resize our buffer and get ready to receive its data
        data->values.resize(data->count);
        MPI_CHECK_RESULT(MPI_Irecv,
                               (&(data->values[0]), data->values.size(),MPI_PACKED,
                                stat.source(), stat.tag(), 
                                MPI_Comm(data->comm), self->m_requests + 1));
      } else
        return std::optional<status>(); // We have not finished yet
    } 

    // Check if we have received the message data
    MPI_CHECK_RESULT(MPI_Test, (self->m_requests + 1, &flag, &stat.m_status));
    if (flag) {
      return stat;
    } else 
      return std::optional<status>();
  } else {
    return std::optional<status>();
  }
}


template<typename T, class A>
request::request(communicator const& comm, int source, int tag, std::vector<T,A>& values, mpl::true_ /*primitive*/)
  : m_data(new detail::dynamic_array_irecv_data<T,A>(comm, source, tag, values)),
    m_handler(handle_dynamic_primitive_array_irecv<T,A>)
{
  m_requests[0] = MPI_REQUEST_NULL;
  m_requests[1] = MPI_REQUEST_NULL;
  std::size_t& count = data<detail::dynamic_array_irecv_data<T,A> >()->count;
  MPI_CHECK_RESULT(MPI_Irecv,
                         (&count, 1, 
                          get_mpi_datatype(count),
                          source, tag, comm, &size_request()));
}

template<typename T, class A>
request
communicator::irecv_vector(int source, int tag, std::vector<T,A>& values, 
                           mpl::true_ primitive) const
{
  return request(*this, source, tag, values, primitive);
}

template<typename T, typename A>
request
communicator::irecv(int source, int tag, std::vector<T,A>& values) const
{
  return irecv_vector(source, tag, values, is_mpi_datatype<T>());
}


//--------------------------------------------------
// send/recv array
  
template<typename T>
request
communicator::array_isend_impl(int dest, int tag, const T* values, int n,
                               mpl::true_) const
{
  request req;
  MPI_CHECK_RESULT(MPI_Isend,
                         (const_cast<T*>(values), n, 
                          get_mpi_datatype<T>(*values),
                          dest, tag, MPI_Comm(*this), req.trivial()));
  return req;
}

// Array isend must send the elements directly
template<typename T>
request communicator::isend(int dest, int tag, const T* values, int n) const
{
  return array_isend_impl(dest, tag, values, n, is_mpi_datatype<T>());
}


template<typename T>
request 
communicator::array_irecv_impl(int source, int tag, T* values, int n, 
                               mpl::true_) const
{
  request req;
  MPI_CHECK_RESULT(MPI_Irecv,
                         (const_cast<T*>(values), n, 
                          get_mpi_datatype<T>(*values),
                          source, tag, MPI_Comm(*this), &req.size_request()));
  return req;
}

// Array receive must receive the elements directly into a buffer.
template<typename T>
request communicator::irecv(int source, int tag, T* values, int n) const
{
  return this->array_irecv_impl(source, tag, values, n, is_mpi_datatype<T>());
}




} } // ns mpi4cpp::mpi
