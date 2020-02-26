#pragma once

#include <type_traits>
#include <typeinfo>

#include "mpi4cpp/datatype_fwd.h"
#include "mpi4cpp/exception.h"


namespace mpi4cpp { namespace mpi { namespace detail {


/// @brief comparison function object for two std::type_info pointers
///
/// is implemented using the before() member function of the std::type_info
/// class
struct type_info_compare
{
  bool operator()(std::type_info const* lhs, std::type_info const* rhs) const
  {
    return lhs->before(*rhs);
  }
};


/// @brief a map of MPI data types, indexed by their type_info
///
///
class mpi_datatype_map
{
  struct implementation;

  implementation *impl;

public:
  mpi_datatype_map();
  ~mpi_datatype_map();

  // getting datatype if primitive type
  // TODO: not used; SFINAE fails
  template <class T>
  MPI_Datatype datatype(const T& x = T(), 
      typename std::enable_if<is_mpi_builtin_datatype<T>::type >::type*  /*unused*/=0)
  {
    return get_mpi_datatype<T>(x);
  }

  /// getting datatype if primitive type with digging deeper from containers
  // TODO: not used; SFINAE fails
  template <class T, std::size_t N>
  MPI_Datatype datatype(const T& x = T(), 
      typename std::enable_if<
        is_mpi_builtin_datatype<
          typename std::remove_all_extents<T>::type
        >::type >::type*  /*unused*/=0)
  {
    return get_mpi_datatype<T>(x);
  }


  /// getting datatype if not primitive
  // TODO: not used; SFINAE fails
  template <class T>
  MPI_Datatype datatype(const T&  /*x*/ =T(), 
      typename std::enable_if<!is_mpi_builtin_datatype<T>::type >::type*  /*unused*/=0 )
  {
    //BOOST_MPL_ASSERT((is_mpi_datatype<T>));

    // check whether the type already exists
    std::type_info const* t = &typeid(T);
    MPI_Datatype datatype = get(t);

    if (datatype == MPI_DATATYPE_NULL) {

      // can not automatically create datatypes
      assert(false);
      
      // need to create a type
      //mpi_datatype_oarchive ar(x);
      //datatype = ar.get_mpi_datatype();
      //set(t, datatype);
    }

    return datatype;
  }
  
  void clear(); 

  MPI_Datatype get(const std::type_info* t);
  void set(const std::type_info* t, MPI_Datatype datatype);
};

/// Retrieve the MPI datatype cache
mpi_datatype_map& mpi_datatype_cache();



} } } // ns mpi4cpp::mpi::detail


#include "mpi_datatype_cache_impl.h"
