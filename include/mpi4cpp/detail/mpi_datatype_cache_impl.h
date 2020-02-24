#pragma once

#include <mpi4cpp/detail/mpi_datatype_cache.h>
#include <map>

namespace mpi4cpp { namespace mpi { namespace detail {

typedef std::map<std::type_info const*,MPI_Datatype,type_info_compare>
    stored_map_type;

struct mpi_datatype_map::implementation
{
  stored_map_type map;
};

inline mpi_datatype_map::mpi_datatype_map()
{
    impl = new implementation();
}

inline void mpi_datatype_map::clear()
{
  // do not free after call to MPI_FInalize
  int finalized=0;
  MPI_CHECK_RESULT(MPI_Finalized,(&finalized));
    
  if (!finalized) {
    // ignore errors in the destructor
    for (auto it=impl->map.begin(); it != impl->map.end(); ++it)
      MPI_Type_free(&(it->second));
  }
}


inline mpi_datatype_map::~mpi_datatype_map()
{
  clear();
  delete impl;
}

inline MPI_Datatype mpi_datatype_map::get(const std::type_info* t)
{
    auto pos = impl->map.find(t);
    if (pos != impl->map.end())
        return pos->second;
    else
        return MPI_DATATYPE_NULL;
}

inline void mpi_datatype_map::set(const std::type_info* t, MPI_Datatype datatype)
{
    impl->map[t] = datatype;
}

inline mpi_datatype_map& mpi_datatype_cache()
{
  static mpi_datatype_map cache;
  return cache;
}


} } }



