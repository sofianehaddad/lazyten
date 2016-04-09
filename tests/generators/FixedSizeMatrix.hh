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
#include "MatrixElement.hh"
#include <linalgwrap/StoredMatrix_i.hh>
#include <rapidcheck.h>

namespace rc {

template <typename Matrix>
struct FixedSizeMatrix {
    static_assert(
          std::is_base_of<
                ::linalgwrap::StoredMatrix_i<typename Matrix::scalar_type>,
                Matrix>::value,
          "Matrix must be a child class of StoredMatrix_i");

    typedef typename Matrix::size_type size_type;
    typedef typename Matrix::scalar_type scalar_type;

    static Gen<Matrix> fixed_size(size_type n_rows, size_type n_cols) {
        auto callable = [=] {
            // allocate memory, but don't initialise
            Matrix m(n_rows, n_cols, false);

            // set to arbitrary values
            for (size_type i = 0; i < m.n_rows() * m.n_cols(); ++i) {
                m[i] = *gen::matrix_element<scalar_type>();
            }

            return m;
        };
        return gen::exec(callable);
    }
};

namespace gen {
template <typename Matrix>
Gen<Matrix> fixed_size(typename Matrix::size_type n_rows,
                       typename Matrix::size_type n_cols) {
    return FixedSizeMatrix<Matrix>::fixed_size(n_rows, n_cols);
}

}  // namespace gen
}  // namespace rc
