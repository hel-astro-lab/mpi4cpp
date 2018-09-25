#pragma once

#include <tuple>
#include <array>
#include <vector>

#include "mpi4cpp/datatype_fwd.h"
#include "mpi4cpp/detail/mpl.h"
#include "mpi4cpp/detail/mpi_datatype_cache.h"


namespace mpi4cpp { namespace mpi {

/**
 *  @brief Type traits that determines if there exists a built-in
 *  MPI data type for a given C++ type.
 */

template<typename T>
struct is_mpi_integer_datatype
  : public mpl::false_ { };

template<typename T>
struct is_mpi_floating_point_datatype
  : public mpl::false_ { };

template<typename T>
struct is_mpi_logical_datatype
  : public mpl::false_ { };

template<typename T>
struct is_mpi_complex_datatype
  : public mpl::false_ { };

template<typename T>
struct is_mpi_byte_datatype
  : public mpl::false_ { };

template<typename T>
struct is_mpi_builtin_datatype
  : mpl::or_<is_mpi_integer_datatype<T>,
             is_mpi_floating_point_datatype<T>,
             is_mpi_logical_datatype<T>,
             is_mpi_complex_datatype<T>,
             is_mpi_byte_datatype<T> >
{ };



/** @brief Type trait that determines if a C++ type can be mapped to
 *  an MPI data type.
 *
 *  This type trait determines if it is possible to build an MPI data
 *  type that represents a C++ data type. When this is the case, @c
 *  is_mpi_datatype derives @c mpl::true_ and the MPI data type will
 *  be accessible via @c get_mpi_datatype.

 *  For any C++ type that maps to a built-in MPI data type (see @c
 *  is_mpi_builtin_datatype), @c is_mpi_data_type is trivially
 *  true. However, any POD ("Plain Old Data") type containing types
 *  that themselves can be represented by MPI data types can itself be
 *  represented as an MPI data type. For instance, a @c point3d class
 *  containing three @c double values can be represented as an MPI
 *  data type. To do so, first make the data type Serializable (using
 *  the Boost.Serialization library); then, specialize the @c
 *  is_mpi_datatype trait for the point type so that it will derive @c
 *  mpl::true_:
 *
 *    @code
 *    namespace boost { namespace mpi {
 *      template<> struct is_mpi_datatype<point>
 *        : public mpl::true_ { };
 *    } }
 *    @endcode
 */
template<typename T>
struct is_mpi_datatype
 : public is_mpi_builtin_datatype<T>
{
};

/** @brief Returns an MPI data type for a C++ type.
 *
 *  The function creates an MPI data type for the given object @c
 *  x. The first time it is called for a class @c T, the MPI data type
 *  is created and cached. Subsequent calls for objects of the same
 *  type @c T return the cached MPI data type.  The type @c T must
 *  allow creation of an MPI data type. That is, it must be
 *  Serializable and @c is_mpi_datatype<T> must derive @c mpl::true_.
 *
 *  For fundamental MPI types, a copy of the MPI data type of the MPI
 *  library is returned.
 *
 *  Note that since the data types are cached, the caller should never
 *  call @c MPI_Type_free() for the MPI data type returned by this
 *  call.
 *
 *  @param x for an optimized call, a constructed object of the type
 *  should be passed; otherwise, an object will be
 *  default-constructed.
 *
 *  @returns The MPI data type corresponding to type @c T.
 */
template<typename T> MPI_Datatype get_mpi_datatype(const T& x)
{
  // TODO
  //BOOST_MPL_ASSERT((is_mpi_datatype<T>));

  return detail::mpi_datatype_cache().datatype(x);
}


//--------------------------------------------------
//
#define MPI4CPP_JOIN( X, Y ) MPI4CPP_DO_JOIN( X, Y )
#define MPI4CPP_DO_JOIN( X, Y ) MPI4CPP_DO_JOIN2(X,Y)
#define MPI4CPP_DO_JOIN2( X, Y ) X##Y

/// INTERNAL ONLY
#define MPI4CPP_DATATYPE(CppType, MPIType, Kind)                         \
template<>                                                              \
inline MPI_Datatype                                                     \
get_mpi_datatype< CppType >(const CppType&) { return MPIType; }         \
                                                                        \
template<>                                                              \
 struct MPI4CPP_JOIN(is_mpi_,MPI4CPP_JOIN(Kind,_datatype))< CppType >     \
: mpl::true_                                                            \
{}

/// INTERNAL ONLY
MPI4CPP_DATATYPE(packed, MPI_PACKED, builtin);
MPI4CPP_DATATYPE(char, MPI_CHAR, builtin);
MPI4CPP_DATATYPE(short, MPI_SHORT, integer);
MPI4CPP_DATATYPE(int, MPI_INT, integer);
MPI4CPP_DATATYPE(long, MPI_LONG, integer);
MPI4CPP_DATATYPE(float, MPI_FLOAT, floating_point);
MPI4CPP_DATATYPE(double, MPI_DOUBLE, floating_point);
MPI4CPP_DATATYPE(long double, MPI_LONG_DOUBLE, floating_point);
MPI4CPP_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR, builtin);
MPI4CPP_DATATYPE(unsigned short, MPI_UNSIGNED_SHORT, integer);
MPI4CPP_DATATYPE(unsigned, MPI_UNSIGNED, integer);
MPI4CPP_DATATYPE(unsigned long, MPI_UNSIGNED_LONG, integer);


#define MPI4CPP_MPI_LIST2(A, B) A, B
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(float, int)>, MPI_FLOAT_INT, 
                   builtin);
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(double, int)>, MPI_DOUBLE_INT, 
                   builtin);
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(long double, int)>,
                   MPI_LONG_DOUBLE_INT, builtin);
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(long, int>), MPI_LONG_INT, 
                   builtin);
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(short, int>), MPI_SHORT_INT, 
                   builtin);
MPI4CPP_DATATYPE(std::pair<MPI4CPP_MPI_LIST2(int, int>), MPI_2INT, builtin);
#undef MPI4CPP_MPI_LIST2


/// specialization of is_mpi_datatype for pairs
template <class T, class U>
struct is_mpi_datatype<std::pair<T,U> >
 : public mpl::and_<is_mpi_datatype<T>,is_mpi_datatype<U> >
{ };


/// specialization of is_mpi_datatype for arrays
template<class T, std::size_t N>
struct is_mpi_datatype<std::array<T, N> >
 : public is_mpi_datatype<T>
{
};

// types that are sometimes not supported and may cause problems

// Define wchar_t specialization of is_mpi_datatype, if possible.
#if (defined(MPI_WCHAR) || (defined(MPI_VERSION) && MPI_VERSION >= 2))
MPI4CPP_DATATYPE(wchar_t, MPI_WCHAR, builtin);
#endif

// Define long long or __int64 specialization of is_mpi_datatype, if possible.
#if (defined(MPI_LONG_LONG_INT) || (defined(MPI_VERSION) && MPI_VERSION >= 2))
MPI4CPP_DATATYPE(long long, MPI_LONG_LONG_INT, builtin);
#elif (defined(MPI_LONG_LONG_INT) || (defined(MPI_VERSION) && MPI_VERSION >= 2))
MPI4CPP_DATATYPE(__int64, MPI_LONG_LONG_INT, builtin); 
#endif

// Define unsigned long long or unsigned __int64 specialization of
// is_mpi_datatype, if possible. We separate this from the check for
// the (signed) long long/__int64 because some MPI implementations
// (e.g., MPICH-MX) have MPI_LONG_LONG_INT but not
// MPI_UNSIGNED_LONG_LONG.
#if (defined(MPI_UNSIGNED_LONG_LONG) \
   || (defined(MPI_VERSION) && MPI_VERSION >= 2))
MPI4CPP_DATATYPE(unsigned long long, MPI_UNSIGNED_LONG_LONG, builtin);
#elif (defined(MPI_UNSIGNED_LONG_LONG) \
   || (defined(MPI_VERSION) && MPI_VERSION >= 2))
MPI4CPP_DATATYPE(unsigned __int64, MPI_UNSIGNED_LONG_LONG, builtin); 
#endif

// Define signed char specialization of is_mpi_datatype, if possible.
#if defined(MPI_SIGNED_CHAR) || (defined(MPI_VERSION) && MPI_VERSION >= 2)
MPI4CPP_DATATYPE(signed char, MPI_SIGNED_CHAR, builtin);
#endif





namespace detail {
  inline MPI_Datatype build_mpi_datatype_for_bool()
  {
    MPI_Datatype type;
    MPI_Type_contiguous(sizeof(bool), MPI_BYTE, &type);
    MPI_Type_commit(&type);
    return type;
  }
}

/// Support for bool. There is no corresponding MPI_BOOL.
template<>
inline MPI_Datatype get_mpi_datatype<bool>(const bool&)
{
  static MPI_Datatype type = detail::build_mpi_datatype_for_bool();
  return type;
}

template<>
struct is_mpi_datatype<bool>
  : mpl::bool_<true>
{};



} } // ns mpi4cpp::mpi
