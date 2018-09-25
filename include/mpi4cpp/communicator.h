#pragma once

#include <optional>
#include <vector>
#include <iterator>
#include <memory>

#include "detail/mpl.h"
#include "exception.h"
#include "status.h"
#include "datatype.h"
#include "request.h"




namespace mpi4cpp { namespace mpi {

/**
 * @brief A constant representing "any process."
 *
 * This constant may be used for the @c source parameter of @c receive
 * operations to indicate that a message may be received from any
 * source.
 */
const int any_source = MPI_ANY_SOURCE;

/**
 * @brief A constant representing "any tag."
 *
 * This constant may be used for the @c tag parameter of @c receive
 * operations to indicate that a @c send with any tag will be matched
 * by the receive.
 */
const int any_tag = MPI_ANY_TAG;


/**
 * @brief Enumeration used to describe how to adopt a C @c MPI_Comm into
 * a Boost.MPI communicator.
 *
 * The values for this enumeration determine how a Boost.MPI
 * communicator will behave when constructed with an MPI
 * communicator. The options are:
 *
 *   - @c comm_duplicate: Duplicate the MPI_Comm communicator to
 *   create a new communicator (e.g., with MPI_Comm_dup). This new
 *   MPI_Comm communicator will be automatically freed when the
 *   Boost.MPI communicator (and all copies of it) is destroyed.
 *
 *   - @c comm_take_ownership: Take ownership of the communicator. It
 *   will be freed automatically when all of the Boost.MPI
 *   communicators go out of scope. This option must not be used with
 *   MPI_COMM_WORLD.
 *
 *   - @c comm_attach: The Boost.MPI communicator will reference the
 *   existing MPI communicator but will not free it when the Boost.MPI
 *   communicator goes out of scope. This option should only be used
 *   when the communicator is managed by the user or MPI library
 *   (e.g., MPI_COMM_WORLD).
 */
//enum comm_create_kind { comm_duplicate, comm_take_ownership, comm_attach };


class communicator
{
  public:

  /**
   * Build a new communicator for @c MPI_COMM_WORLD.
   *
   * Constructs a communicator that attaches to @c
   * MPI_COMM_WORLD. This is the equivalent of constructing with
   * @c (MPI_COMM_WORLD, comm_attach).
   */
  communicator();

  //communicator(const MPI_Comm& comm, comm_create_kind kind);
  //communicator(const communicator& comm, const boost::mpi::group& subgroup);


  /**
   * @brief Determine the rank of the executing process in a
   * communicator.
   *
   * This routine is equivalent to @c MPI_Comm_rank.
   *
   *   @returns The rank of the process in the communicator, which
   *   will be a value in [0, size())
   */
  int rank() const;

  /**
   * @brief Determine the number of processes in a communicator.
   *
   * This routine is equivalent to @c MPI_Comm_size.
   *
   *   @returns The number of processes in the communicator.
   */
  int size() const;

  /** @brief Determine if this communicator is valid for
   * communication.
   *
   * Evaluates @c true in a boolean context if this communicator is
   * valid for communication, i.e., does not represent
   * MPI_COMM_NULL. Otherwise, evaluates @c false.
   */
  operator bool() const { return (bool)comm_ptr; }


  //boost::mpi::group group() const;

  /**
   * @brief Access the MPI communicator associated with a Boost.MPI
   * communicator.
   *
   * This routine permits the implicit conversion from a Boost.MPI
   * communicator to an MPI communicator.
   *
   *   @returns The associated MPI communicator.
   */
  operator MPI_Comm() const;

  /** Abort all tasks in the group of this communicator.
   *
   *  Makes a "best attempt" to abort all of the tasks in the group of
   *  this communicator. Depending on the underlying MPI
   *  implementation, this may either abort the entire program (and
   *  possibly return @p errcode to the environment) or only abort
   *  some processes, allowing the others to continue. Consult the
   *  documentation for your MPI implementation. This is equivalent to
   *  a call to @c MPI_Abort
   *
   *  @param errcode The error code to return from aborted processes.
   *  @returns Will not return.
   */
  void abort(int errcode) const;


  //--------------------------------------------------
  // Point-to-point communication

  /**
   *  @brief Send a message to another process without any data.
   *
   *  This routine executes a potentially blocking send of a message
   *  to another process. The message contains no extra data, and can
   *  therefore only be received by a matching call to @c recv().
   *
   *  @param dest The process rank of the remote process to which
   *  the message will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   */
  void send(int dest, int tag) const;

  /**
   *  @brief Receive a message from a remote process without any data.
   *
   *  This routine blocks until it receives a message from the process
   *  @p source with the given @p tag.
   *
   *  @param source The process that will be sending the message. This
   *  will either be a process rank within the communicator or the
   *  constant @c any_source, indicating that we can receive the
   *  message from any process.
   *
   *  @param tag The tag that matches a particular kind of message
   *  sent by the source process. This may be any tag value permitted
   *  by @c send. Alternatively, the argument may be the constant @c
   *  any_tag, indicating that this receive matches a message with any
   *  tag.
   *
   *  @returns Information about the received message.
   */
  status recv(int source, int tag) const;


  /**
   *  @brief Send data to another process.
   *
   *  This routine executes a potentially blocking send with tag @p tag
   *  to the process with rank @p dest. It can be received by the
   *  destination process with a matching @c recv call.
   *
   *  The given @p value must be suitable for transmission over
   *  MPI. There are several classes of types that meet these
   *  requirements:
   *
   *    - Types with mappings to MPI data types: If @c
   *    is_mpi_datatype<T> is convertible to @c mpl::true_, then @p
   *    value will be transmitted using the MPI data type
   *    @c get_mpi_datatype<T>(). All primitive C++ data types that have
   *    MPI equivalents, e.g., @c int, @c float, @c char, @c double,
   *    etc., have built-in mappings to MPI data types. You may turn a
   *    Serializable type with fixed structure into an MPI data type by
   *    specializing @c is_mpi_datatype for your type.
   *
   *    - Serializable types: Any type that provides the @c serialize()
   *    functionality required by the Boost.Serialization library can be
   *    transmitted and received.
   *
   *    - Packed archives and skeletons: Data that has been packed into
   *    an @c mpi::packed_oarchive or the skeletons of data that have
   *    been backed into an @c mpi::packed_skeleton_oarchive can be
   *    transmitted, but will be received as @c mpi::packed_iarchive and
   *    @c mpi::packed_skeleton_iarchive, respectively, to allow the
   *    values (or skeletons) to be extracted by the destination process.
   *
   *    - Content: Content associated with a previously-transmitted
   *    skeleton can be transmitted by @c send and received by @c
   *    recv. The receiving process may only receive content into the
   *    content of a value that has been constructed with the matching
   *    skeleton.
   *
   *  For types that have mappings to an MPI data type (including the
   *  content of a type), an invocation of this routine will result in
   *  a single MPI_Send call. For variable-length data, e.g.,
   *  serialized types and packed archives, two messages will be sent
   *  via MPI_Send: one containing the length of the data and the
   *  second containing the data itself.
   * 
   *  Std::vectors of MPI data type
   *  are considered variable size, e.g. their number of elements is 
   *  unknown and must be transmitted (although the serialization process
   *  is skipped). You can use the array specialized versions of 
   *  communication methods is both sender and receiver know the vector 
   *  size.
   *  
   *  Note that the transmission mode for variable-length data is an 
   *  implementation detail that is subject to change.
   *
   *  @param dest The rank of the remote process to which the data
   *  will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   *  @param value The value that will be transmitted to the
   *  receiver. The type @c T of this value must meet the aforementioned
   *  criteria for transmission. 
   */
  template<typename T>
  void send(int dest, int tag, const T& value) const;

  template<typename T, typename A>
  void send(int dest, int tag, const std::vector<T,A>& value) const;

  /**
   *  @brief Send an array of values to another process.
   *
   *  This routine executes a potentially blocking send of an array of
   *  data with tag @p tag to the process with rank @p dest. It can be
   *  received by the destination process with a matching array @c
   *  recv call.
   *
   *  If @c T is an MPI datatype, an invocation of this routine will
   *  be mapped to a single call to MPI_Send, using the datatype @c
   *  get_mpi_datatype<T>().
   *
   *  @param dest The process rank of the remote process to which
   *  the data will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   *  @param values The array of values that will be transmitted to the
   *  receiver. The type @c T of these values must be mapped to an MPI
   *  data type.
   *
   *  @param n The number of values stored in the array. The destination
   *  process must call receive with at least this many elements to
   *  correctly receive the message.
   */
  template<typename T>
  void send(int dest, int tag, const T* values, int n) const;

  /**
   * @brief Receive data from a remote process.
   *
   * This routine blocks until it receives a message from the process @p
   * source with the given @p tag. The type @c T of the @p value must be
   * suitable for transmission over MPI, which includes serializable
   * types, types that can be mapped to MPI data types (including most
   * built-in C++ types), packed MPI archives, skeletons, and content
   * associated with skeletons; see the documentation of @c send for a
   * complete description.
   *
   *   @param source The process that will be sending data. This will
   *   either be a process rank within the communicator or the
   *   constant @c any_source, indicating that we can receive the
   *   message from any process.
   *
   *   @param tag The tag that matches a particular kind of message sent
   *   by the source process. This may be any tag value permitted by @c
   *   send. Alternatively, the argument may be the constant @c any_tag,
   *   indicating that this receive matches a message with any tag.
   *
   *   @param value Will contain the value of the message after a
   *   successful receive. The type of this value must match the value
   *   transmitted by the sender, unless the sender transmitted a packed
   *   archive or skeleton: in these cases, the sender transmits a @c
   *   packed_oarchive or @c packed_skeleton_oarchive and the
   *   destination receives a @c packed_iarchive or @c
   *   packed_skeleton_iarchive, respectively.
   *
   *   @returns Information about the received message.
   */
  template<typename T>
  status recv(int source, int tag, T& value) const;

  template<typename T, typename A>
  status recv(int source, int tag, std::vector<T,A>& value) const;


  /**
   * @brief Receive an array of values from a remote process.
   *
   * This routine blocks until it receives an array of values from the
   * process @p source with the given @p tag. If the type @c T is 
   *
   *   @param source The process that will be sending data. This will
   *   either be a process rank within the communicator or the
   *   constant @c any_source, indicating that we can receive the
   *   message from any process.
   *
   *   @param tag The tag that matches a particular kind of message sent
   *   by the source process. This may be any tag value permitted by @c
   *   send. Alternatively, the argument may be the constant @c any_tag,
   *   indicating that this receive matches a message with any tag.
   *
   *   @param values Will contain the values in the message after a
   *   successful receive. The type of these elements must match the
   *   type of the elements transmitted by the sender.
   *
   *   @param n The number of values that can be stored into the @p
   *   values array. This shall not be smaller than the number of
   *   elements transmitted by the sender.
   *
   *   @throws std::range_error if the message to be received contains
   *   more than @p n values.
   *
   *   @returns Information about the received message.
   */
  template<typename T>
  status recv(int source, int tag, T* values, int n) const;


  // We're sending/receiving a vector with associated MPI datatype.
  // We need to send/recv the size and then the data and make sure 
  // blocking and non blocking method agrees on the format.

  template<typename T, typename A>
  void send_vector(int dest, int tag, const std::vector<T,A>& value, 
		   mpl::true_) const;
  template<typename T, typename A>
  status recv_vector(int source, int tag, std::vector<T,A>& value,
		     mpl::true_) const;


  protected:

  /**
   * INTERNAL ONLY
   *
   * Function object that frees an MPI communicator and deletes the
   * memory associated with it. Intended to be used as a deleter with
   * shared_ptr.
   */
  struct comm_free
  {
    void operator()(MPI_Comm* comm) const
    {
      assert( comm != 0 );
      assert(*comm != MPI_COMM_NULL);
      int finalized;
      MPI_CHECK_RESULT(MPI_Finalized, (&finalized));
      if (!finalized)
        MPI_CHECK_RESULT(MPI_Comm_free, (comm));
      delete comm;
    }
  };


  //--------------------------------------------------

  /**
   * We're sending a type that has an associated MPI datatype, so we
   * map directly to that datatype.
   */
  template<typename T>
  void send_impl(int dest, int tag, const T& value, mpl::true_) const;

  /**
   * We're receiving a type that has an associated MPI datatype, so we
   * map directly to that datatype.
   */
  template<typename T>
  status recv_impl(int source, int tag, T& value, mpl::true_) const;

  //--------------------------------------------------

  /**
   * We're sending an array of a type that has an associated MPI
   * datatype, so we map directly to that datatype.
   */
  template<typename T>
  void 
  array_send_impl(int dest, int tag, const T* values, int n, mpl::true_) const;

  /**
   * We're receiving an array of a type that has an associated MPI
   * datatype, so we map directly to that datatype.
   */
  template<typename T>
  status 
  array_recv_impl(int source, int tag, T* values, int n, mpl::true_) const;

  //--------------------------------------------------
  // Non-blocking communications

  public:
  /**
   *  @brief Send a message to another process without any data
   *  without blocking.
   *
   *  This routine is functionally identical to the @c send method for
   *  sends with no data, except that @c isend will not block while
   *  waiting for the message to be transmitted. Instead, a request
   *  object will be immediately returned, allowing one to query the
   *  status of the communication or wait until it has completed.
   *
   *  @param dest The process rank of the remote process to which
   *  the message will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   *
   *  @returns a @c request object that describes this communication.
   */
  request isend(int dest, int tag) const;
  
  /**
   *  @brief Initiate receipt of a message from a remote process that
   *  carries no data.
   *
   *  This routine initiates a receive operation for a message from
   *  process @p source with the given @p tag that carries no data.
   *
   *    @param source The process that will be sending the message. This
   *    will either be a process rank within the communicator or the
   *    constant @c any_source, indicating that we can receive the
   *    message from any process.
   *
   *    @param tag The tag that matches a particular kind of message
   *    sent by the source process. This may be any tag value permitted
   *    by @c send. Alternatively, the argument may be the constant @c
   *    any_tag, indicating that this receive matches a message with any
   *    tag.
   *
   *    @returns a @c request object that describes this communication.
   */
  request irecv(int source, int tag) const;




  /**
   *  @brief Send a message to a remote process without blocking.
   *
   *  The @c isend method is functionality identical to the @c send
   *  method and transmits data in the same way, except that @c isend
   *  will not block while waiting for the data to be
   *  transmitted. Instead, a request object will be immediately
   *  returned, allowing one to query the status of the communication
   *  or wait until it has completed.
   *
   *  @param dest The rank of the remote process to which the data
   *  will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   *  @param value The value that will be transmitted to the
   *  receiver. The type @c T of this value must meet the aforementioned
   *  criteria for transmission. If modified before transmited, the 
   *  modification may or may not be transmited.
   *
   *  @returns a @c request object that describes this communication.
   */
  template<typename T>
  request isend(int dest, int tag, const T& value) const;

  /**
   *  @brief Prepare to receive a message from a remote process.
   *
   *  The @c irecv method is functionally identical to the @c recv
   *  method and receive data in the same way, except that @c irecv
   *  will not block while waiting for data to be
   *  transmitted. Instead, it immediately returns a request object
   *  that allows one to query the status of the receive or wait until
   *  it has completed.
   *
   *   @param source The process that will be sending data. This will
   *   either be a process rank within the communicator or the
   *   constant @c any_source, indicating that we can receive the
   *   message from any process.
   *
   *   @param tag The tag that matches a particular kind of message sent
   *   by the source process. This may be any tag value permitted by @c
   *   send. Alternatively, the argument may be the constant @c any_tag,
   *   indicating that this receive matches a message with any tag.
   *
   *   @param value Will contain the value of the message after a
   *   successful receive. The type of this value must match the value
   *   transmitted by the sender, unless the sender transmitted a packed
   *   archive or skeleton: in these cases, the sender transmits a @c
   *   packed_oarchive or @c packed_skeleton_oarchive and the
   *   destination receives a @c packed_iarchive or @c
   *   packed_skeleton_iarchive, respectively.
   *
   *   @returns a @c request object that describes this communication.
   */
  template<typename T>
  request irecv(int source, int tag, T& value) const;



  /**
   *  @brief Send an array of values to another process without
   *  blocking.
   *
   *  This routine is functionally identical to the @c send method for
   *  arrays except that @c isend will not block while waiting for the
   *  data to be transmitted. Instead, a request object will be
   *  immediately returned, allowing one to query the status of the
   *  communication or wait until it has completed.
   *
   *  @param dest The process rank of the remote process to which
   *  the data will be sent.
   *
   *  @param tag The tag that will be associated with this message. Tags
   *  may be any integer between zero and an implementation-defined
   *  upper limit. This limit is accessible via @c environment::max_tag().
   *
   *  @param values The array of values that will be transmitted to the
   *  receiver. The type @c T of these values must be mapped to an MPI
   *  data type.
   *
   *  @param n The number of values stored in the array. The destination
   *  process must call receive with at least this many elements to
   *  correctly receive the message.
   *
   *  @returns a @c request object that describes this communication.
   */
  template<typename T>
  request isend(int dest, int tag, const T* values, int n) const;

  template<typename T, class A>
  request isend(int dest, int tag, const std::vector<T,A>& values) const;


  /**
   * @brief Initiate receipt of an array of values from a remote process.
   *
   * This routine initiates a receive operation for an array of values
   * transmitted by process @p source with the given @p tag. 
   *
   *    @param source The process that will be sending data. This will
   *    either be a process rank within the communicator or the
   *    constant @c any_source, indicating that we can receive the
   *    message from any process.
   *
   *    @param tag The tag that matches a particular kind of message sent
   *    by the source process. This may be any tag value permitted by @c
   *    send. Alternatively, the argument may be the constant @c any_tag,
   *    indicating that this receive matches a message with any tag.
   *
   *    @param values Will contain the values in the message after a
   *    successful receive. The type of these elements must match the
   *    type of the elements transmitted by the sender.
   *
   *    @param n The number of values that can be stored into the @p
   *    values array. This shall not be smaller than the number of
   *    elements transmitted by the sender.
   *
   *    @returns a @c request object that describes this communication.
   */
  template<typename T>
  request irecv(int source, int tag, T* values, int n) const;

  template<typename T, typename A>
  request irecv(int source, int tag, std::vector<T,A>& values) const;
  
  private:

  /**
   * We're sending a type that has an associated MPI datatype, so we
   * map directly to that datatype.
   */
  template<typename T>
  request isend_impl(int dest, int tag, const T& value, mpl::true_) const;

  /*
   * We're receiving a type that has an associated MPI datatype, so we
   * map directly to that datatype.
   */
  template<typename T>
  request irecv_impl(int source, int tag, T& value, mpl::true_) const;


  /**
   * We're sending an array of a type that has an associated MPI
   * datatype, so we map directly to that datatype.
   */
  template<typename T>
  request 
  array_isend_impl(int dest, int tag, const T* values, int n, 
                   mpl::true_) const;

  /**
   * We're receiving a type that has an associated MPI datatype, so we
   * map directly to that datatype.
   */
  template<typename T>
  request 
  array_irecv_impl(int source, int tag, T* values, int n, mpl::true_) const;


  // We're sending/receivig a vector with associated MPI datatype.
  // We need to send/recv the size and then the data and make sure 
  // blocking and non blocking method agrees on the format.
  template<typename T, typename A>
  request irecv_vector(int source, int tag, std::vector<T,A>& values, 
                       mpl::true_) const;

  template<typename T, class A>
  request isend_vector(int dest, int tag, const std::vector<T,A>& values,
                       mpl::true_) const;


  
  public:
  /**
   * @brief Waits until a message is available to be received.
   *
   * This operation waits until a message matching (@p source, @p tag)
   * is available to be received. It then returns information about
   * that message. The functionality is equivalent to @c MPI_Probe. To
   * check if a message is available without blocking, use @c iprobe.
   *
   *   @param source Determine if there is a message available from
   *   this rank. If @c any_source, then the message returned may come
   *   from any source.
   *
   *   @param tag Determine if there is a message available with the
   *   given tag. If @c any_tag, then the message returned may have any
   *   tag.
   *
   *   @returns Returns information about the first message that
   *   matches the given criteria.
   */
  //status probe(int source = any_source, int tag = any_tag) const;

  /**
   * @brief Determine if a message is available to be received.
   *
   * This operation determines if a message matching (@p source, @p
   * tag) is available to be received. If so, it returns information
   * about that message; otherwise, it returns immediately with an
   * empty optional. The functionality is equivalent to @c
   * MPI_Iprobe. To wait until a message is available, use @c wait.
   *
   *   @param source Determine if there is a message available from
   *   this rank. If @c any_source, then the message returned may come
   *   from any source.
   *
   *   @param tag Determine if there is a message available with the
   *   given tag. If @c any_tag, then the message returned may have any
   *   tag.
   *
   *   @returns If a matching message is available, returns
   *   information about that message. Otherwise, returns an empty
   *   @c boost::optional.
   */
  //optional<status>
  //iprobe(int source = any_source, int tag = any_tag) const;


#ifdef barrier
  // Linux defines a function-like macro named "barrier". So, we need
  // to avoid expanding the macro when we define our barrier()
  // function. However, some C++ parsers (Doxygen, for instance) can't
  // handle this syntax, so we only use it when necessary.
  void (barrier)() const;
#else
  /**
   * @brief Wait for all processes within a communicator to reach the
   * barrier.
   *
   * This routine is a collective operation that blocks each process
   * until all processes have entered it, then releases all of the
   * processes "simultaneously". It is equivalent to @c MPI_Barrier.
   */
  void barrier() const;
#endif





 protected:
  std::shared_ptr<MPI_Comm> comm_ptr;

};

/**
 * @brief Determines whether two communicators are identical.
 *
 * Equivalent to calling @c MPI_Comm_compare and checking whether the
 * result is @c MPI_IDENT.
 *
 * @returns True when the two communicators refer to the same
 * underlying MPI communicator.
 */
bool operator==(const communicator& comm1, const communicator& comm2);


/**
 * @brief Determines whether two communicators are different.
 *
 * @returns @c !(comm1 == comm2)
 */
inline bool operator!=(const communicator& comm1, const communicator& comm2)
{
  return !(comm1 == comm2);
}


} } // ns mpi4cpp::mpi

// communication specific implementations
#include "communicator_impl.h"

// point-to-point communication implementations
#include "point2point_impl.h"

// nonblocking communication patterns
#include "nonblocking_impl.h"

