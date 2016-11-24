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

#ifdef LINALGWRAP_HAVE_ARPACK
#include "eigensolver_tests.hh"
#include <linalgwrap/Arpack.hh>
#include <linalgwrap/SmallMatrix.hh>

namespace linalgwrap {
namespace tests {
using namespace rc;

TEST_CASE("ArpackEigensolver", "[ArpackEigensolver]") {
    using namespace eigensolver_tests;
    typedef SmallMatrix<double> matrix_type;

    /* The filter functor to filter out problems which make no sense
     * for us here*/
    auto filter = [](const EigensolverTestProblemBase<matrix_type>& problem) {
        // Arpack cannot deal with cases where the dimensionality is equal
        // to the number of eigenpairs to be seeked
        if (problem.n_ep >= problem.dim) return false;
        //
        // Arpack is bad at finding the smallest magnitude eigenvalues
        if (problem.params.at<std::string>(EigensolverBaseKeys::which, "SR") ==
            std::string("SM"))
            return false;

        return true;
    };

    SECTION("Real hermitian normal problems") {
        typedef EigensolverTestProblem<matrix_type,
                                       /* Hermitian= */ true,
                                       /* general= */ false>
              tprob_type;
        typedef ArpackEigensolver<typename tprob_type::prob_type> solver_type;

        TestProblemRunner<tprob_type>(
              DefaultSolveFunctor<tprob_type, solver_type>())
              .run_matching(filter);
    }  // real hermitian normal problems

    // TODO test real hermitian generalised problems
    // TODO test real non-hermitian problems
    // TODO test real non-hermitian generalised problems

}  // ArpackEigensolver

}  // tests
}  // linalgwrap
#endif