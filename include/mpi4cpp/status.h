#pragma once

#include <optional>


namespace mpi4cpp { namespace mpi {


class status 
{
  public:

  status() : m_count(-1) { }

  status(MPI_Status const& s) : m_status(s), m_count(-1) {}

  /**
   * Retrieve the source of the message.
   */
  int source() const { return m_status.MPI_SOURCE; }

  /**
   * Retrieve the message tag.
   */
  int tag() const { return m_status.MPI_TAG; }

  /**
   * Retrieve the error code.
   */
  int error() const { return m_status.MPI_ERROR; }

  /**
   * Determine whether the communication associated with this object
   * has been successfully cancelled.
  */
  bool cancelled() const
  {
    int flag = 0;
    MPI_CHECK_RESULT(MPI_Test_cancelled, (&m_status, &flag));
    return flag != 0;
  }


  /**
   * Determines the number of elements of type @c T contained in the
   * message. The type @c T must have an associated data type, i.e.,
   * @c is_mpi_datatype<T> must derive @c mpl::true_. In cases where
   * the type @c T does not match the transmitted type, this routine
   * will return an empty @c optional<int>.
   *
   * @returns the number of @c T elements in the message, if it can be
   * determined.
   */
  //template<typename T> optional<int> count() const;


  /**
   * References the underlying @c MPI_Status
   */
  operator       MPI_Status&()       { return m_status; }

  /**
   * References the underlying @c MPI_Status
   */
  operator const MPI_Status&() const { return m_status; }


  /// INTERNAL ONLY
  mutable MPI_Status m_status;
  mutable int m_count;

  friend class communicator;
  //friend class request;

};





} } // ns mpi4cpp::mpi
