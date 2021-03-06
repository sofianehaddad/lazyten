
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
#include "matrix_tests.hh"
#include <catch.hpp>
#include <lazyten/Exceptions.hh>

namespace lazyten {
namespace tests {
using namespace rc;

/** Namespace for default tests for stored matrices */
namespace stored_matrix_tests {

template <typename Matrix>
class TestingLibrary {
 public:
  typedef Matrix matrix_type;
  typedef typename matrix_type::size_type size_type;
  typedef typename matrix_type::scalar_type scalar_type;

  /** Construct a testing library instance in order to check
   *  all basic functionality of a stored matrix
   *
   *  \param prefix A prefix to use in the rapidcheck description string
   *  \param tolerance Numeric tolerance used for comparison in (some) checks.
   *                   The checks for which machine epsilon is used as
   *                   tolerance are:
   *  - Copying a stored matrix
   *  - Setting and getting elements via [], () or iterator
   */
  TestingLibrary(std::string prefix = "") : m_prefix{prefix}, m_gen{argsgen} {}

  void run_checks() const;

 private:
  // The testing library and caller type
  typedef matrix_type data_type;   // The args we generate
  typedef matrix_type model_type;  // The model we generate
  typedef matrix_tests::ComparativeTests<model_type, matrix_type> comptests;
  typedef RCTestableGenerator<model_type, matrix_type, data_type> gen_type;

  /** Identity operation on the matrix */
  static constexpr matrix_type identity(matrix_type m) { return m; };

  /** Argument generation */
  static constexpr matrix_type argsgen() {
    return *gen::numeric_tensor<matrix_type>().as("Matrix");
  };

  void once_test_initialiser_list_constructor() const;

  std::string m_prefix;
  gen_type m_gen;
};

//
// ---------------------------------------
//

template <typename Matrix>
void TestingLibrary<Matrix>::once_test_initialiser_list_constructor() const {
  matrix_type m{{11.0, 12.0}, {21.0, 22.0}, {31.0, 32.0}};

  CHECK((m.n_rows() == 3));
  CHECK((m.n_cols() == 2));

  for (size_type i = 0; i < m.n_rows(); ++i) {
    for (size_type j = 0; j < m.n_cols(); ++j) {
      CHECK((m(i, j) == 10. * (i + 1) + j + 1));
    }
  }

#ifdef DEBUG
  CHECK_THROWS_AS((matrix_type{{1.0, 2.0}, {1.0}}), krims::ExcSizeMismatch);
#endif
}

template <typename Matrix>
void TestingLibrary<Matrix>::run_checks() const {
  // Shorter aliases:
  const NumCompAccuracyLevel eps = NumCompAccuracyLevel::MachinePrecision;
  const NumCompAccuracyLevel high = NumCompAccuracyLevel::Higher;
  const NumCompAccuracyLevel low = NumCompAccuracyLevel::Lower;
  const NumCompAccuracyLevel sloppy = NumCompAccuracyLevel::Sloppy;
  const NumCompAccuracyLevel supersloppy = NumCompAccuracyLevel::SuperSloppy;
  static constexpr bool cplx = krims::IsComplexNumber<scalar_type>::value;

  // Test construction from initialiser list
  once_test_initialiser_list_constructor();

  // Test copying stored matrices
  // TODO change all calls to
  CHECK(m_gen.run_test(m_prefix + "Test copying stored matrices", comptests::test_copy,
                       eps));

  // Read-only element access
  CHECK(rc::check(m_prefix + "Element access via () and []",
                  m_gen.generate(comptests::test_element_access, eps)));
  CHECK(rc::check(m_prefix + "Element access via extract_block",
                  m_gen.generate(comptests::test_extract_block, eps)));
  CHECK(m_gen.run_test(m_prefix + "Element access via transposed extract_block",
                       comptests::test_extract_transpose_block, eps));
  CHECK(rc::check(m_prefix + "Read-only iterator of small matrices",
                  m_gen.generate(comptests::test_readonly_iterator, eps)));

  // Read-write element access
  CHECK(rc::check(m_prefix + "Altering elements via ()",
                  m_gen.generate(comptests::test_setting_elements_indexed, eps)));
  CHECK(rc::check(m_prefix + "Altering elements via []",
                  m_gen.generate(comptests::test_setting_elements_vectorised, eps)));
  CHECK(rc::check(m_prefix + "Altering elements via iterator",
                  m_gen.generate(comptests::test_readwrite_iterator, eps)));

  // Standard operations and norms
  CHECK(rc::check(m_prefix + "l1 norm", m_gen.generate(comptests::test_norm_l1, eps)));

  CHECK(rc::check(m_prefix + "linf norm", m_gen.generate(comptests::test_norm_linf)));

  CHECK(rc::check(m_prefix + "Frobenius norm",
                  m_gen.generate(comptests::test_norm_frobenius)));

  CHECK(m_gen.run_test(m_prefix + "Elementwise functions", comptests::test_elementwise));

  if (!cplx) {
    CHECK(m_gen.run_test(m_prefix + "Min and max function", comptests::test_minmax, eps));
  }

  CHECK(rc::check(m_prefix + "accumulate function",
                  m_gen.generate(comptests::test_accumulate, sloppy)));

  CHECK(rc::check(m_prefix + "trace calculation",
                  m_gen.generate(comptests::test_trace, low)));

  // Basic operations (+,-,scaling)
  CHECK(rc::check(m_prefix + "Multiplication by scalar",
                  m_gen.generate(comptests::test_multiply_scalar)));
  CHECK(rc::check(m_prefix + "Divide by scalar",
                  m_gen.generate(comptests::test_divide_scalar)));
  CHECK(rc::check(m_prefix + "Add a matrix",
                  m_gen.generate(comptests::template test_add<Matrix>)));

  // Apply and matrix multiplication
  CHECK(m_gen.run_test(m_prefix + "Apply to MultiVector",
                       comptests::template test_apply_to<typename Matrix::vector_type>,
                       low));
  CHECK(m_gen.run_test(
        m_prefix + "Transpose apply to MultiVector",
        comptests::template test_transpose_apply_to<typename Matrix::vector_type>, low));
  CHECK(m_gen.run_test(m_prefix + "Test mmult", comptests::test_mmult, low));
  CHECK(m_gen.run_test(m_prefix + "Test transposed mmult",
                       comptests::test_transposed_mmult, low));
  CHECK(m_gen.run_test(m_prefix + "Matrix multiplication",
                       comptests::template test_multiply_by<Matrix>, supersloppy));
}

}  // namespace stored_matrix_tests
}  // namespace tests
}  // namespace lazyten
