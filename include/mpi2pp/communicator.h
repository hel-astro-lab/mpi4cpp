#pragma once

#include <optional>
#include <vector>
#include <iterator>
#include <memory>

#include "exception.h"
#include "status.h"




namespace mpi2pp { namespace mpi {

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
  // Blocking communication

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


} } // ns mpi2pp::mpi



#include "communicator_impl.h"
