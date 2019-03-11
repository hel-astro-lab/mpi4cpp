#pragma once

#include <exception>

#include "exception.h"


namespace mpi4cpp { namespace mpi {

using nonstd::optional;
using nonstd::nullopt;


inline request::request()
  : m_data(), m_handler(0)
{
  m_requests[0] = MPI_REQUEST_NULL;
  m_requests[1] = MPI_REQUEST_NULL;
}

//inline request::request(request& other)
//{
//  std::cout<<"calling user-defined copy operator\n";
//  assert(false); // test optimization with this
//
//  m_handler = std::move(other.m_handler);
//  m_data    = std::move(other.m_data);
//  m_requests[0] = std::move(other.m_requests[0]);
//  m_requests[1] = std::move(other.m_requests[1]);
//}

inline request::request(const request& other)
{
  // std::vector slow_path goes here
  //std::cout<<"calling user-defined const copy operator\n";
  //assert(false); // test optimization with this

  m_handler = std::move(other.m_handler);
  m_data    = std::move(other.m_data);
  m_requests[0] = std::move(other.m_requests[0]);
  m_requests[1] = std::move(other.m_requests[1]);

  m_touch = 1;

  //other.m_handler = 0;
  //other.m_data = nullptr;
  //other.m_requests[0] = MPI_REQUEST_NULL;
  //other.m_requests[1] = MPI_REQUEST_NULL;
}

inline request::request(request&& other)
{
  // std::vector calls mainly this...
  //std::cout<<"calling user-defined move operator\n";
  m_handler = std::move(other.m_handler);
  m_data    = std::move(other.m_data);
  m_requests[0] = std::move(other.m_requests[0]);
  m_requests[1] = std::move(other.m_requests[1]);
  m_touch = 2;

  // empty out the other
  other.m_handler = 0;
  other.m_data = nullptr;
  other.m_requests[0] = MPI_REQUEST_NULL;
  other.m_requests[1] = MPI_REQUEST_NULL;
  other.m_touch = 3;
}

inline request& request::operator=(request& /*other*/)
{
  std::cout<<"calling user-defined copy assignment\n";
  assert(false);

  return *this;
}

inline request& request::operator=(const request& /*other*/)
{
  std::cout<<"calling user-defined const copy assignment\n";
  assert(false);

  return *this;
}

inline request& request::operator=(request&& other)
{
  //std::cout<<"calling user-defined move assignment\n";
  m_handler = std::move(other.m_handler);
  m_data    = std::move(other.m_data);
  m_requests[0] = std::move(other.m_requests[0]);
  m_requests[1] = std::move(other.m_requests[1]);
  m_touch = 4;

  // empty out the other
  other.m_handler = 0;
  other.m_data = nullptr;
  other.m_requests[0] = MPI_REQUEST_NULL;
  other.m_requests[1] = MPI_REQUEST_NULL;
  other.m_touch = 5;

  //other.m_handler = m_handler;
  //other.m_data    = m_data;
  //other.m_requests[0] = m_requests[0];
  //other.m_requests[1] = m_requests[1];

  return *this;
}


//std::optional< std::reference_wrapper<MPI_Request> >
inline MPI_Request*
request::trivial() {
  if ((!bool(m_handler) && m_requests[1] == MPI_REQUEST_NULL)) {
    return &m_requests[0];
  } else {
    return nullptr;
  }
}


inline bool
request::active() const {
  return m_requests[0] != MPI_REQUEST_NULL || m_requests[1] != MPI_REQUEST_NULL;
}


inline status 
request::wait()
{
  if (m_handler) {
    // This request is a receive for a serialized type. Use the
    // handler to wait for completion.
    return *m_handler(this, ra_wait);
  } else if (m_requests[1] == MPI_REQUEST_NULL) {
    // This request is either a send or a receive for a type with an
    // associated MPI datatype, or a serialized datatype that has been
    // packed into a single message. Just wait on the one receive/send
    // and return the status to the user.

    //if (m_requests[0] == MPI_REQUEST_NULL) {
    //  std::cout << "wait(): oops, req0 is null \n";
    //  //assert(false);

    //  return status();
    //}

    status result;
    MPI_CHECK_RESULT(MPI_Wait, (&m_requests[0], &result.m_status));
    return result;
  } else {
    
    //if (m_requests[0] == MPI_REQUEST_NULL) {
    //  std::cout << "wait(): oops2, req0 is null \n";
    //  assert(false);
    //}
    
    // This request is a send of a serialized type, broken into two
    // separate messages. Complete both sends at once.
    MPI_Status stats[2];
    int error_code = MPI_Waitall(2, m_requests, stats);
    if (error_code == MPI_ERR_IN_STATUS) {
      // Dig out which status structure has the error, and use that
      // one when throwing the exception.
      if (stats[0].MPI_ERROR == MPI_SUCCESS 
          || stats[0].MPI_ERROR == MPI_ERR_PENDING)
        //throw std::exception("MPI_Waitall", stats[1].MPI_ERROR);
        throw MPI_Waitall_Error();
      else
        //throw std::exception("MPI_Waitall", stats[0].MPI_ERROR);
        throw MPI_Waitall_Error();
    } else if (error_code != MPI_SUCCESS) {
      // There was an error somewhere in the MPI_Waitall call; throw
      // an exception for it.
      //throw std::exception("MPI_Waitall", error_code);
      throw MPI_Waitall_Error();
    } 

    // No errors. Returns the first status structure.
    status result;
    result.m_status = stats[0];
    return result;
  }
}


inline optional<status> 
request::test()
{
  if (m_handler) {
    // This request is a receive for a serialized type. Use the
    // handler to test for completion.
    return m_handler(this, ra_test);
  } else if (m_requests[0] == MPI_REQUEST_NULL && m_requests[1] != MPI_REQUEST_NULL) {
    std::cout << "test: funny switch that should not be there\n";
    assert(false);

  } else if (m_requests[1] == MPI_REQUEST_NULL) {
    // This request is either a send or a receive for a type with an
    // associated MPI datatype, or a serialized datatype that has been
    // packed into a single message. Just test the one receive/send
    // and return the status to the user if it has completed.
    status result;
    int flag = 0;
    MPI_CHECK_RESULT(MPI_Test, (&m_requests[0], &flag, &result.m_status));
    return flag != 0? optional<status>(result) : optional<status>();
  } else {
    // This request is a send of a serialized type, broken into two
    // separate messages. We only get a result if both complete.
    MPI_Status stats[2];
    int flag = 0;
    int error_code = MPI_Testall(2, m_requests, &flag, stats);
    if (error_code == MPI_ERR_IN_STATUS) {
      // Dig out which status structure has the error, and use that
      // one when throwing the exception.
      if (stats[0].MPI_ERROR == MPI_SUCCESS 
          || stats[0].MPI_ERROR == MPI_ERR_PENDING)
        //throw std::exception("MPI_Testall", stats[1].MPI_ERROR);
        throw MPI_Testall_Error();
      else
        //throw std::exception("MPI_Testall", stats[0].MPI_ERROR);
        throw MPI_Testall_Error();
    } else if (error_code != MPI_SUCCESS) {
      // There was an error somewhere in the MPI_Testall call; throw
      // an exception for it.
      //throw std::exception("MPI_Testall", error_code);
      throw MPI_Testall_Error();
    }

    // No errors. Returns the second status structure if the send has
    // completed.
    if (flag != 0) {
      status result;
      result.m_status = stats[1];
      return result;
    } else {
      return optional<status>();
    }
  }
}

inline void 
request::cancel()
{
  if (m_handler) {
    m_handler(this, ra_cancel);
  } else {
    MPI_CHECK_RESULT(MPI_Cancel, (&m_requests[0]));
    if (m_requests[1] != MPI_REQUEST_NULL)
    {
      MPI_CHECK_RESULT(MPI_Cancel, (&m_requests[1]));
    }
  }
}


inline 
request::~request()
{

  //if (m_handler) {
  //  m_handler(this, ra_wait);
  //else {

  //if (active()) {
  //  std::cout << "ZZZ: - - - - active request destroyed\n";
  //  assert(false);
  //}

  if (!m_handler) {
    // always have to pass
    if (m_requests[0] != MPI_REQUEST_NULL) {
      if (m_requests[0]) {
        // test if this is finished
        status result;
        int flag = 0;
        std::cout << " error is: " << m_requests[0] << "\n";
        MPI_CHECK_RESULT(MPI_Test, (&m_requests[0], &flag, &result.m_status));
        if (!flag) {
          std::cout << "XXX: - - - - error detected with message destructor; message 1 is not destroyed "<< m_touch << "\n";
          assert(false);
        }
      }
    }
      
    // can be null
    if (m_requests[1] != MPI_REQUEST_NULL) {
      if (m_requests[1]) {
        status result;
        int flag = 0;
        MPI_CHECK_RESULT(MPI_Test, (&m_requests[1], &flag, &result.m_status));
        if (!flag) {
          std::cout << "YYY:     - - - - error detected with message destructor; message 2 is not destroyed " << m_touch << \n";
          assert(false);
        }

      }
    }

  }

}







} } // end namespace mpi4cpp::mpi
