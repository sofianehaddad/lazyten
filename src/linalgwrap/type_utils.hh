//
// Copyright (C) 2016 by the linalgwrap authors
//
// This file is part of linalgwrap.
//
// linalgwrap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// linalgwrap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with linalgwrap. If not, see <http://www.gnu.org/licenses/>.
//

#pragma once
#include <complex>
#include <type_traits>

namespace linalgwrap {

namespace detail {
/** Workaround make_void class required in some versions of gcc
 *  (most notably gcc 4.9) to get the ``using void_t = void`` idiom
 *  to work.
 *  For gcc 5 and most clang versions this could be avoided and else
 *  we could use the simpler version:
 *  ```
 *  template <typename... Ts>
 *  using void_t = void.
 *  ```
 *  See https://stackoverflow.com/questions/35753920/
 *  why-does-the-void-t-detection-idiom-not-work-with-gcc-4-9
 *  for details.
 *
 *  The basic reason is that the simpler code above does not actually
 *  make use of the Ts, so their substitution is not a failure, hence
 *  SFINAE is not triggered.
 */
template <typename... Ts>
struct make_void {
    using type = void;
};
}  // namespace detail

/* Using statement we need for some SFINAE
 * (substitution failure is not an error)
 *
 * See also the documentation of \t detail::make_void, why we make
 * it so complicated instead of just a plain ``using void_t = void``.
 * */
template <typename... Ts>
using void_t = typename detail::make_void<Ts...>::type;

//@{
/** \brief helper struct to extract the underlying real type from a
 *         potentially complex scalar type.
 *
 * The real type is accessible via the member type "type".
 *  */
template <typename Scalar>
struct RealTypeOf {
    static_assert(std::is_scalar<Scalar>::value,
                  "RealTypeOf can only operate on scalar types.");
    typedef Scalar type;
};

template <typename Scalar>
struct RealTypeOf<std::complex<Scalar>> {
    typedef Scalar type;
};
//@}

//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether T is a complex scalar type
 *
 * The definition is done using SFINAE, such that even for types not having a
 * typedef value_type this expression is valid.
 *  */
template <typename T, typename = void>
struct IsComplexScalar : public std::false_type {};

template <typename T>
struct IsComplexScalar<T, void_t<typename T::value_type>>
      : public std::is_same<std::complex<typename T::value_type>, T> {};
//@}

}  // namespace linalgwrap
