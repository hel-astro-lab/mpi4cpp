#pragma once

namespace mpi4cpp { namespace mpi {
  namespace mpl {

//--------------------------------------------------

// Bool
/*
template< bool C_ > struct bool_;

typedef bool_<true> true_;
typedef bool_<false> false_;
*/

//--------------------------------------------------
// BOOLEAN
struct Integer;

template <typename T, T v>
struct integer_c {
  using value_type = T;
  static constexpr value_type value = v;
  constexpr operator value_type() const { return value; }
  constexpr value_type operator()() const { return value; }
  using type = integer_c;
  using mpl_datatype = Integer;
};

//! Alias to `integer_c<T, v>`; provided for backward compatibility.
template <typename T, T v>
using integral_c = integer_c<T, v>;

template <bool B>
using bool_ = integer_c<bool, B>;

using true_ = bool_<true>;
using false_ = bool_<false>;

//--------------------------------------------------
// OR
  
/* Equivalent to `std::is_same`; except it only has a nested `::%value`
 * and a nested `::%type`.
 */
template <typename T, typename U>
struct std_is_same {
    static constexpr bool value = false;
    using type = std_is_same;
};

template <typename T>
struct std_is_same<T, T> {
    static constexpr bool value = true;
    using type = std_is_same;
};

/*!
* Alias to the most efficient non short-circuiting logical or.
*/
namespace detail { namespace logical_or {
template <bool ...>
struct strict_is_same_impl;

/*!
* Strict (non short-circuiting) logical or implemented with `is_same`.
*
* All credit goes to Roland Bock for the technique.
*/
template <typename ...xs>
using strict_is_same = bool_<
            !std_is_same<
            strict_is_same_impl<(bool)xs::type::value...>,
            strict_is_same_impl<(xs::type::value, false)...>
        >::value
      >;

template <typename ...xs>
using strict = strict_is_same<xs...>;

} } // ns detail::logical_or


/*!
 * Returns the result of short-circuit _logical or_ (`||`) on its
 * arguments.
 *
 * Arguments must be `StaticConstant`s that will be unboxed from left to
 * right in a short-circuiting fashion. If no arguments are provided,
 * `or_` is equivalent to `false_`.
 */
//template <typename ...xs>
//struct or_;
//
//
//template <typename T, T ...v>
//struct or_<integer_c<T, v>...>
//  : strict<bool_<(bool)v>...>
//{ };
////: detail::logical_or::strict<bool_<(bool)v>...>
//
//template <typename T, T ...v>
//struct and_<integer_c<T, v>...> // DeMorgan
//  : bool_<!strict<bool_<!v>...>::value>
//{ };
//: bool_<!detail::logical_or::strict<bool_<!v>...>::value>


//--------------------------------------------------
// AND
  
/*!
 * Returns the result of short-circuit _logical and_ (`&&`) on its
 * arguments.
 *
 * Arguments must be `StaticConstant`s that will be unboxed from left to
 * right in a short-circuiting fashion. If no arguments are provided,
 * `and_` is equivalent to `true_`.
 */

namespace logical_detail {

  template <bool cond>
  struct if_impl;

  template <>
  struct if_impl<true> {
  template <typename Then, typename Else>
    using result = Then;
  };

  template <>
  struct if_impl<false> {
  template <typename Then, typename Else>
    using result = Else;
  };

  //! Equivalent to `if_<bool_<Condition>, Then, Else>`.
  template <bool Condition, typename Then, typename Else>
  using if_c = typename if_impl<Condition>::template result<Then, Else>;

  template <typename x, typename y>
  struct and2
    : bool_<
           (bool)if_c<(bool)x::type::value, y, x>::type::value
            >
  { };

  template <typename x, typename y>
  struct or2
    : bool_<
            (bool)if_c<(bool)x::type::value, x, y>::type::value
            >
  { };
} // end namespace logical_detail


namespace detail { namespace right_folds {

  /*!
   * Right fold over a parameter pack with loop unrolling.
   *
   * The metafunction is unlifted for performance.
  */
  template <template <typename ...> class f, typename state, typename ...xs>
  struct variadic_unrolled;
  
  template <
      template <typename ...> class f,
      typename state
      , typename x0, typename x1, typename x2, typename x3, typename x4, typename x5,
      typename ...xs>
  struct variadic_unrolled<f, state , x0, x1, x2, x3, x4, x5, xs...>
      : f<x0, f<x1, f<x2, f<x3, f<x4, f<x5, variadic_unrolled<f, state, xs...>>>>>>>
  { };
  
  template <
    template <typename ...> class f,
    typename state
  >
  struct variadic_unrolled<f, state  >
      : state
  { };

  template <
      template <typename ...> class f,
      typename state
      , typename x0
  >
  struct variadic_unrolled<f, state , x0 >
      : f<x0, state>
  { };

  template <
      template <typename ...> class f,
      typename state
      , typename x0, typename x1
  >
  struct variadic_unrolled<f, state , x0, x1 >
      : f<x0, f<x1, state>>
  { };

  template <
      template <typename ...> class f,
      typename state
      , typename x0, typename x1, typename x2
  >
  struct variadic_unrolled<f, state , x0, x1, x2 >
      : f<x0, f<x1, f<x2, state>>>
  { };

  template <
      template <typename ...> class f,
      typename state
      , typename x0, typename x1, typename x2, typename x3
  >
  struct variadic_unrolled<f, state , x0, x1, x2, x3 >
      : f<x0, f<x1, f<x2, f<x3, state>>>>
  { };

  template <
      template <typename ...> class f,
      typename state
      , typename x0, typename x1, typename x2, typename x3, typename x4
  >
  struct variadic_unrolled<f, state , x0, x1, x2, x3, x4 >
      : f<x0, f<x1, f<x2, f<x3, f<x4, state>>>>>
  { };

  /*!
   * Alias to the most efficient variadic right fold.
   */
  template <template <typename ...> class f, typename state, typename ...xs>
  using variadic = variadic_unrolled<f, state, xs...>;

}}// end namespace detail::right_fold

template <typename ...xs>
    struct or_;

template <typename ...xs>
    struct and_;

template <typename x>
    struct not_;

template <typename ...xs>
struct and_
    : detail::right_folds::variadic<logical_detail::and2, true_, xs...>
{ };

template <typename ...xs>
struct or_
    : detail::right_folds::variadic<logical_detail::or2, false_, xs...>
{ };

template <typename x>
struct not_
    : bool_<!x::type::value>
{ };

template <typename Condition, typename Then, typename Else>
struct if_ :
    logical_detail::if_impl<(bool)Condition::type::value>::
    template result<Then, Else>
{ };



  } // ns mpl
} } // ns mpi4cpp::mpi

