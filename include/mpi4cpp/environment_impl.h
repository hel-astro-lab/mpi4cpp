#include "environment.h"
#include "exception.h"

#include <cassert>
#include <string>
#include <ostream>


namespace mpi2pp { namespace mpi {


environment::environment(bool abort_on_exception)
  : i_initialized(false),
    abort_on_exception(abort_on_exception)
{
  if (!initialized()) {
    MPI_CHECK_RESULT(MPI_Init, (0, 0));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


environment::environment(int& argc, char** &argv, bool abort_on_exception)
  : i_initialized(false),
    abort_on_exception(abort_on_exception)
{
  if (!initialized()) {
    MPI_CHECK_RESULT(MPI_Init, (&argc, &argv));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


environment::environment(int& argc, char** &argv, threading::level mt_level,
                         bool abort_on_exception)
  : i_initialized(false),
    abort_on_exception(abort_on_exception)
{
  // It is not clear that we can pass null in MPI_Init_thread.
  int dummy_thread_level = 0;
  if (!initialized()) {
    MPI_CHECK_RESULT(MPI_Init_thread, 
                           (&argc, &argv, int(mt_level), &dummy_thread_level));
    i_initialized = true;
  }
  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
}


environment::~environment()
{
  if (i_initialized) {
    if (std::uncaught_exception() && abort_on_exception) {
      abort(-1);
    } else if (!finalized()) {

      // TODO
      //detail::mpi_datatype_cache().clear();

      MPI_CHECK_RESULT(MPI_Finalize, ());
    }
  }
}

void environment::abort(int errcode)
{
  MPI_CHECK_RESULT(MPI_Abort, (MPI_COMM_WORLD, errcode));
}

bool environment::initialized()
{
  int flag;
  MPI_CHECK_RESULT(MPI_Initialized, (&flag));
  return flag != 0;
}

bool environment::finalized()
{
  int flag;
  MPI_CHECK_RESULT(MPI_Finalized, (&flag));
  return flag != 0;
}


int environment::max_tag()
{
  int* max_tag_value;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                  (MPI_COMM_WORLD, MPI_TAG_UB, &max_tag_value, &found));
  assert(found != 0);
  return *max_tag_value - num_reserved_tags;
}


int environment::collectives_tag()
{
  return max_tag() + 1;
}


std::optional<int> environment::host_rank()
{
  int* host;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                         (MPI_COMM_WORLD, MPI_HOST, &host, &found));
  if (!found || *host == MPI_PROC_NULL)
    return std::optional<int>();
  else
    return *host;
}

std::optional<int> environment::io_rank()
{
  int* io;
  int found = 0;

  MPI_CHECK_RESULT(MPI_Comm_get_attr,
                         (MPI_COMM_WORLD, MPI_IO, &io, &found));
  if (!found || *io == MPI_PROC_NULL)
    return std::optional<int>();
  else
    return *io;
}


std::string environment::processor_name()
{
  char name[MPI_MAX_PROCESSOR_NAME];
  int len;

  MPI_CHECK_RESULT(MPI_Get_processor_name, (name, &len));
  return std::string(name, len);
}


threading::level environment::thread_level()
{
  int level;

  MPI_CHECK_RESULT(MPI_Query_thread, (&level));
  return static_cast<threading::level>(level);
}


bool environment::is_main_thread()
{
  int isit;

  MPI_CHECK_RESULT(MPI_Is_thread_main, (&isit));
  return static_cast<bool>(isit);
}

std::pair<int, int> environment::version()
{
  int major, minor;
  MPI_CHECK_RESULT(MPI_Get_version, (&major, &minor));
  return std::make_pair(major, minor);
}






} } // ns mpi2pp::mpi
