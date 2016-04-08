#ifndef LINALG_MATRIX_I_HPP_
#define LINALG_MATRIX_I_HPP_

#include "linalgwrap/Constants.hh"
#include "linalgwrap/DefaultMatrixIterator.hh"
#include "linalgwrap/Exceptions.hh"
#include "linalgwrap/SmallMatrix.hh"
#include "linalgwrap/Subscribable.hh"
#include "linalgwrap/type_utils.hh"
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>

namespace linalgwrap {

template <typename IteratorCore>
class MatrixIterator;

template <typename Matrix, bool Constness>
class MatrixIteratorDefaultCore;

/** \brief Abstract matrix interface class
 *
 * This interface and hence all classes derived from it are subscribable using
 * the SubscriptionPointer class. This should be used very little and only when
 * other means (e.g. using shared pointers) is not possible.
 * */
template <typename Scalar>
class Matrix_i : public Subscribable {
  public:
    typedef size_t size_type;
    typedef Scalar scalar_type;

    //! The iterator type (a const iterator)
    typedef DefaultMatrixConstIterator<Matrix_i<Scalar>> iterator;

    //! The const iterator type
    typedef DefaultMatrixConstIterator<Matrix_i<Scalar>> const_iterator;

    /** \name Matrix constructor, destructor and assignment */
    ///@{
    virtual ~Matrix_i() = default;
    Matrix_i() = default;
    Matrix_i(const Matrix_i&) = default;
    Matrix_i(Matrix_i&&) = default;
    Matrix_i& operator=(const Matrix_i&) = default;
    Matrix_i& operator=(Matrix_i&&) = default;
    ///@}

    /** \name Matrix properties
     *        Access to properties common to all matrices
     */
    ///@{
    /** \brief Number of rows of the matrix */
    virtual size_type n_rows() const = 0;

    /** \brief Number of columns of the matrix */
    virtual size_type n_cols() const = 0;
    ///@}

    /** \name Data access
     *        Access to matrix data
     */
    ///@{
    /** \brief return an element of the matrix    */
    virtual scalar_type operator()(size_type row, size_type col) const = 0;

    /** \brief return an element of the vectorised matrix object
     *
     * Access the element in row-major ordering (i.e. the matrix is
     * traversed row by row)
     */
    virtual scalar_type operator[](size_type i) const;
    ///@}

    /** \name Iterators
     */
    ///@{
    /** Return an iterator to the beginning */
    iterator begin();

    /** Return a const_iterator to the beginning */
    const_iterator begin() const;

    /** Return a const_iterator to the beginning */
    const_iterator cbegin() const;

    /** Return an iterator to the end */
    iterator end();

    /** Return a const_iterator to the end */
    const_iterator end() const;

    /** Return a const_iterator to the end */
    const_iterator cend() const;
    ///@}

    /** \name Standard operations
     */
    ///@{
    /** Calculate the (signed) sum of all matrix entries. */
    virtual scalar_type accumulate() const {
        return std::accumulate(begin(), end(), Constants<scalar_type>::zero);
    }
    ///@}
};

//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether T is a stored matrix
 *
 * The definition is done using SFINAE, such that even for types not having a
 * typedef scalar_type this expression is valid.
 *  */
template <typename T, typename = void>
struct IsMatrix : public std::false_type {};

template <typename T>
struct IsMatrix<T, void_t<typename T::scalar_type>>
      : public std::is_base_of<Matrix_i<typename T::scalar_type>, T> {};
//@}

/** \brief Simple output operator, that plainly shows all entries of
 *  the Matrix one by one.
 *
 *  Rows are seperated by a newline and entries by spaces.
 *  The last row is not terminated by a newline character.
 *  */
template <typename Scalar>
std::ostream& operator<<(std::ostream& o, const Matrix_i<Scalar>& m);

//
// ---------------------------------------------------------------
//

//
// Matrix_i
//
template <typename Scalar>
typename Matrix_i<Scalar>::scalar_type Matrix_i<Scalar>::operator[](
      size_type i) const {
    // Check that we do not overshoot.
    assert_range(0, i, n_cols() * n_rows());

    const size_type i_row = i / n_cols();
    const size_type i_col = i % n_cols();
    return (*this)(i_row, i_col);
}

template <typename Scalar>
typename Matrix_i<Scalar>::iterator Matrix_i<Scalar>::begin() {
    return iterator(*this, {0, 0});
}

template <typename Scalar>
typename Matrix_i<Scalar>::const_iterator Matrix_i<Scalar>::begin() const {
    return cbegin();
}

template <typename Scalar>
typename Matrix_i<Scalar>::const_iterator Matrix_i<Scalar>::cbegin() const {
    return const_iterator(*this, {0, 0});
}

template <typename Scalar>
typename Matrix_i<Scalar>::iterator Matrix_i<Scalar>::end() {
    return iterator(*this);
}

template <typename Scalar>
typename Matrix_i<Scalar>::const_iterator Matrix_i<Scalar>::end() const {
    return cend();
}

template <typename Scalar>
typename Matrix_i<Scalar>::const_iterator Matrix_i<Scalar>::cend() const {
    return const_iterator(*this);
}

//
// Out of scope
//
template <typename Scalar>
std::ostream& operator<<(std::ostream& o, const Matrix_i<Scalar>& m) {
    typedef typename Matrix_i<Scalar>::size_type size_type;

    for (size_type i = 0; i < m.n_rows(); ++i) {
        for (size_type j = 0; j < m.n_cols(); ++j) {
            o << std::setw(15) << m(i, j);
        }

        if (i == m.n_rows() - 1) break;
        o << std::endl;
    }

    return o;
}

}  // namespace linalg
#endif  // LINALG_MATRIX_I_HPP_