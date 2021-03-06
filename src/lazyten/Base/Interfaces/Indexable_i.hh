//
// Copyright (C) 2016-17 by the lazyten authors
//
// This file is part of lazyten.
//
// lazyten is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lazyten is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with lazyten. If not, see <http://www.gnu.org/licenses/>.
//

#pragma once
#include "lazyten/Constants.hh"
#include <iostream>
#include <krims/ExceptionSystem.hh>
#include <krims/Subscribable.hh>
#include <krims/TypeUtils.hh>
#include <numeric>

namespace lazyten {

// TODO someday ... template <unsigned int R, typename Scalar>
// class Indexable_i : public krims::Subscribable {

/** \brief Abstract marker interface class to represent an indexable object
 * into a stride of memory of a certain scalar type.
 *
 * Implementing classes are further asked to implement the following
 * methods:
 *  - ``begin()``, ``cbegin()``  Return an iterator or a constant iterator
 *    to the beginning of the stride of memory.
 *  - ``end()``, ``cend()``   Return an iterator/constant iterator to the
 *    position past-the-end of the stride of memory.
 *
 * The following types should also be defined:
 *  - iterator  The type returned by begin()
 *  - const_iterator The type returned by cbegin()
 *
 * \tparam Scalar scalar type of the indexable
 */
template <typename Scalar>
class Indexable_i : public krims::Subscribable {
 public:
  /** The scalar type to use. All elements are stored in terms of the scalar
   * type */
  typedef Scalar scalar_type;

  //@{
  /** For compatibility with the STL (We can use Indexables like STL containers */
  typedef scalar_type value_type;
  typedef scalar_type& reference;
  typedef const scalar_type& const_reference;
  typedef scalar_type* pointer;
  typedef const scalar_type* const_pointer;
  //@}

  /** The real type to use. All norms and similar are returned in terms of the
   * real type */
  typedef typename krims::RealTypeOf<Scalar>::type real_type;

  /** The size type to use. All counting and indexing is done in terms of the
   * size type */
  typedef size_t size_type;

  // /** The shape type to use. The shape of the indexable object (extent in
  // the
  //  * dimensions) is returned in terms of this type and indexing using
  //  operator()
  //  *  uses this type as well. */
  // typedef const Index<R> shape_type;
  //
  // /** The rank of this indexable */
  // constexpr unsigned int rank() { return R; }
  //
  // /** \brief Return the shape of the indexable memory */
  // virtual shape_type shape() const = 0;
  //
  // /** \brief return an element of the indexable memory */
  // virtual scalar_type operator()(shape_type i) const = 0;

  /** \brief Return the number of elements of the indexable memory */
  virtual size_type n_elem() const = 0;

  /** Size of the indexable, i.e. how many elements are present */
  virtual size_type size() const { return n_elem(); }

  /** \brief return an element of the indexable memory */
  virtual scalar_type operator[](size_type i) const = 0;

  /** \brief Default constructors and assignment */
  //@{
  Indexable_i() = default;
  virtual ~Indexable_i() = default;
  Indexable_i(const Indexable_i&) = default;
  Indexable_i(Indexable_i&&) = default;
  Indexable_i& operator=(const Indexable_i&) = default;
  Indexable_i& operator=(Indexable_i&&) = default;
  //@}
};

//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether T is an indexable object.
 *
 * The definition is done using SFINAE, such that even for types not having a
 * typedef scalar_type this expression is valid.
 *  */
template <typename T, typename = void>
struct IsIndexable : public std::false_type {};

template <typename T>
struct IsIndexable<T, krims::VoidType<typename T::scalar_type>>
      : public std::is_base_of<Indexable_i<typename T::scalar_type>, T> {};
//@}

/** Type alias to obtain the scalar type of a valid Indexable object */
template <typename T>
using ValidIndexableScalarT =
      typename std::enable_if<IsIndexable<T>::value, typename T::scalar_type>::type;

/** Type alias to assert that the index type is valid */
template <typename T>
using ValidIndexableT = typename std::enable_if<IsIndexable<T>::value, T>::type;

/** \brief Simple output operator, that plainly shows all entries of
 *  the Matrix one by one.
 *
 *  Rows are separated by a newline and entries by spaces.
 *  The last row is not terminated by a newline character.
 *  */
template <typename Scalar>
std::ostream& operator<<(std::ostream& o, const Indexable_i<Scalar>& ible) {
  for (size_t i = 0; i < ible.n_elem(); ++i) {
    o << ible[i] << " ";
  }

  // TODO extend
  // assert_implemented(false);
  // io::MatrixPrinter().print(m, o);
  return o;
}

/** Convert an indexable with only one element into this scalar element */
template <typename Scalar>
Scalar as_scalar(const Indexable_i<Scalar>& i) {
  assert_size(i.n_elem(), 1);
  return i[0];
}

}  // namespace lazyten
