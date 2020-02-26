#pragma once

#include "environment.h"
#include "exception.h"

#include <cassert>
#include <string>
#include <ostream>


namespace mpi4cpp { namespace mpi {

using nonstd::optional;
using nonstd::nullopt;


inline environment::environment(bool abort_on_exception)
  : 
    abort_on_exception(abort_on_exception)
{
  if(!initialized()) {
    int flag=-2;
    MPI_Initialized(&flag);
    //std::cout << "mpi_init " << flag << "\n";

    MPI_CHECK_RESULT(MPI_Init, (nullptr, nullptr));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


inline environment::environment(int& argc, char** &argv, bool abort_on_exception)
  : i_initialized(false),
    abort_on_exception(abort_on_exception)
{
  if (!initialized()) {
    //std::cout << "mpi_init2\n";
    MPI_CHECK_RESULT(MPI_Init, (&argc, &argv));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


inline environment::environment(int& argc, char** &argv, threading::level mt_level,
                         bool abort_on_exception)
  : i_initialized(false),
    abort_on_exception(abort_on_exception)
{
  // It is not clear that we can pass null in MPI_Init_thread.
  int dummy_thread_level = 0;
  if (!initialized()) {
    //std::cout << "mpi_init3\n";
    MPI_CHECK_RESULT(MPI_Init_thread, 
                           (&argc, &argv, int(mt_level), &dummy_thread_level));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


inline environment::~environment()
{
  if (i_initialized) {
    if (std::uncaught_exception() && abort_on_exception) {
      abort(-1);
    } else if (!finalized()) {

      detail::mpi_datatype_cache().clear();

      MPI_CHECK_RESULT(MPI_Finalize, ());
    }
  }
}

inline void 
environment::abort(int errcode)
{
  MPI_CHECK_RESULT(MPI_Abort, (MPI_COMM_WORLD, errcode));
}

inline bool 
environment::initialized()
{
  int flag;
  MPI_CHECK_RESULT(MPI_Initialized, (&flag));
  return flag != 0;
}

inline bool 
environment::finalized()
{
  int flag;
  MPI_CHECK_RESULT(MPI_Finalized, (&flag));
  return flag != 0;
}


inline int 
environment::max_tag()
{
  int* max_tag_value;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                  (MPI_COMM_WORLD, MPI_TAG_UB, &max_tag_value, &found));
  assert(found != 0);
  return *max_tag_value - num_reserved_tags;
}


inline int 
environment::collectives_tag()
{
  return max_tag() + 1;
}


inline optional<int> 
environment::host_rank()
{
  int* host;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                         (MPI_COMM_WORLD, MPI_HOST, &host, &found));
  if ((found == 0) || *host == MPI_PROC_NULL)
    return optional<int>();
  else
    return *host;
}

inline optional<int> 
environment::io_rank()
{
  int* io;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                         (MPI_COMM_WORLD, MPI_IO, &io, &found));
  if ((found == 0) || *io == MPI_PROC_NULL)
    return optional<int>();
  else
    return *io;
}


inline std::string 
environment::processor_name()
{
  char name[MPI_MAX_PROCESSOR_NAME];
  int len;

  MPI_CHECK_RESULT(MPI_Get_processor_name, (name, &len));
  return std::string(name, len);
}


inline threading::level 
environment::thread_level()
{
  int level;

  MPI_CHECK_RESULT(MPI_Query_thread, (&level));
  return static_cast<threading::level>(level);
}


inline bool 
environment::is_main_thread()
{
  int isit;

  MPI_CHECK_RESULT(MPI_Is_thread_main, (&isit));
  return static_cast<bool>(isit);
}


inline std::pair<int, int> 
environment::version()
{
  int major, minor;
  MPI_CHECK_RESULT(MPI_Get_version, (&major, &minor));
  return std::make_pair(major, minor);
}






} } // ns mpi4cpp::mpi
