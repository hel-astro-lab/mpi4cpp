#pragma once

//#include <optional>
#include "nonstd/optional.hpp"
#include <functional>

namespace mpi4cpp { namespace mpi {

using nonstd::optional;
using nonstd::nullopt;

class status;
class communicator;

/**
 *  @brief A request for a non-blocking send or receive.
 *
 *  This structure contains information about a non-blocking send or
 *  receive and will be returned from @c isend or @c irecv,
 *  respectively.
 */
class request 
{
 public:
  /**
   *  Constructs a NULL request.
   */
  request();

  /**
   *  Constructs request for complex data.
   */
  template<typename T> 
  request(communicator const& comm, int source, int tag, T& value);

  /**
   *  Constructs request for array of complex data.
   */  
  template<typename T> 
  request(communicator const& comm, int source, int tag, T* value, int n);

  /**
   *  Constructs request for array of primitive data.
   */
  template<typename T, class A> 
  request(communicator const& comm, int source, int tag, std::vector<T,A>& values, mpl::true_ primitive);

  /**
   *  Wait until the communication associated with this request has
   *  completed, then return a @c status object describing the
   *  communication.
   */
  status wait();

  /**
   *  Determine whether the communication associated with this request
   *  has completed successfully. If so, returns the @c status object
   *  describing the communication. Otherwise, returns an empty @c
   *  optional<> to indicate that the communication has not completed
   *  yet. Note that once @c test() returns a @c status object, the
   *  request has completed and @c wait() should not be called.
   */
  optional<status> test();

  /**
   *  Cancel a pending communication, assuming it has not already been
   *  completed.
   */
  void cancel();
  
  /**
   * The trivial MPI request implementing this request, provided it's trivial.
   * Probably irrelevant to most users.
   *
   * XXX: due to std::optional<T&> battle in C++-17 atm I simplified this
   * to T* for the sake of getting it work!
   */
  //std::optional<MPI_Request&> trivial();
  //std::optional< std::reference_wrapper<MPI_Request> > trivial();
  MPI_Request* trivial();

  /**
   * For two steps requests, that need to first send the size, then the payload,
   * access to the size request.
   * Probably irrelevant to most users.
   */
  MPI_Request& size_request() { return m_requests[0]; }

  /**
   * For two steps requests, that need to first send the size, then the payload,
   * access to the size request.
   * Probably irrelevant to most users.
   */
  MPI_Request& payload_request() { return m_requests[1]; }

  /**
   * Is this request potentially pending ?
   */
  bool active() const;

  template<class T> std::shared_ptr<T>    data() { return std::static_pointer_cast<T>(m_data); }
  template<class T> void set_data(std::shared_ptr<T>& d) { m_data = d; }

 private:
  enum request_action { ra_wait, ra_test, ra_cancel };
  typedef optional<status> (*handler_type)(request* self, 
                                           request_action action);

  /**
   * Handles the non-blocking receive of a serialized value.
   */
  template<typename T>
  static optional<status> 
  handle_serialized_irecv(request* self, request_action action);

  /**
   * Handles the non-blocking receive of an array of  serialized values.
   */
  template<typename T>
  static optional<status> 
  handle_serialized_array_irecv(request* self, request_action action);

   /**
   * Handles the non-blocking receive of a dynamic array of primitive values.
   */
  template<typename T, class A>
  static optional<status> 
  handle_dynamic_primitive_array_irecv(request* self, request_action action);

 private:
  MPI_Request           m_requests[2];
  std::shared_ptr<void> m_data;
  handler_type          m_handler{nullptr};
};

} } // end namespace mpi4cpp::mpi

#include "request_impl.h"
