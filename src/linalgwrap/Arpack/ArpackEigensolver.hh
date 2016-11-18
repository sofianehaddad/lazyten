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
#ifdef LINALGWRAP_HAVE_ARPACK
#include "detail.hh"
#include "linalgwrap/Base/Solvers.hh"
#include "linalgwrap/Exceptions.hh"
#include "linalgwrap/PtrVector.hh"

namespace linalgwrap {

template <typename Eigenproblem>
struct ArpackEigensolverState : public EigensolverStateBase<Eigenproblem> {
    typedef EigensolverStateBase<Eigenproblem> base_type;
    typedef typename base_type::eproblem_type eproblem_type;
    typedef typename base_type::scalar_type scalar_type;
    typedef typename base_type::size_type size_type;

    /** The current state of the ARPACK ido parameter */
    int ido;

    /** The current iparam parameter as used by ARPACK
     *
     * \note quite some of the n_ ... int references
     *  refer into this array at appropriate places */
    std::array<int, 11> iparam;

    /** The number of converged ritz values */
    const int& n_conv_ritz;

    /** The number of operator applies (A*x or Diag*x) */
    const int& n_mtx_applies;

    /** The number of applies B*x */
    const int& n_bmtx_applies;

    /** The number of reorthogonalisation steps */
    const int& n_reortho_steps;

    /** The pointer to the current residual vector */
    std::shared_ptr<std::vector<scalar_type>> resid_ptr;

    /** Is the residual from a previous run or a fresh one? */
    bool have_old_residual;

    /** The pointer to the reverse communication workbench */
    std::shared_ptr<std::vector<scalar_type>> workd_ptr;

    /** Get the number of Arnoldi iterations performed.
     *
     * \note
     * Since Arpack does not support obtaining an intermediate
     *  iteration count, the value this function returns
     *  is 0 unless the iteration has converged.
     **/
    size_type n_iter_count() const {
        if (ido == 99) {
            // return Arpack value
            return static_cast<size_type>(iparam[2]);
        } else {
            return 0u;
        }
    }

    /** Setup the initial state from an eigenproblem to solve */
    ArpackEigensolverState(const eproblem_type problem)
          : base_type(std::move(problem)),
            ido(0),
            iparam(),
            n_conv_ritz(iparam[4]),
            n_mtx_applies(iparam[8]),
            n_bmtx_applies(iparam[9]),
            n_reortho_steps(iparam[10]),
            resid_ptr{new std::vector<scalar_type>(
                  base_type::eigenproblem().dim(), 0.0)},
            have_old_residual{false},
            workd_ptr{new std::vector<scalar_type>(
                  3 * base_type::eigenproblem().dim(), 0.0)} {
        // Zero the array:
        std::fill(iparam.begin(), iparam.end(), 0);
    }
};

DefException1(ExcArpackInvalidIdo, int,
              << "Arpack ido parameter " << arg1
              << " is unknown to us. Check ARPACK documentation.");
DefSolverException2(
      ExcArpackInfo, std::string, function, int, info,
      << "Arpack function " << function << " returned with info value " << info
      << ". Check Arpack documentation for the meaning of this value.");

DefSolverException2(ExcArpackCannotFindMoreEigenpairs, size_t, desired,
                    size_t, found,
                    << "Arpack has found all eigenpairs it can possibly find. "
                    << desired << " eigenpairs were desired, but " << found
                    << " eigenpairs were found.");

/** Class which contains all ParameterMap keys which are understood
 *  by the ArpackSolver update_control_params as static string
 *  members.
 *  See their doc strings for the types required. */
struct ArpackEigensolverKeys : public EigensolverBaseKeys {
    /** Maximum number of iterations. Type: size_t */
    static const std::string max_iter;

    /** Number of Arnoldi vectors. Type: size_t */
    static const std::string n_arnoldi_vectors;

    /** Arpack mode to use, Type: int */
    static const std::string mode;
};

/** \brief Arpack eigensolver class
 *
 * ## Control parameters and their default values
 *   - max_iter: Maximum number of iterations. Default: 100
 *   - which:    Which eigenvalues to target. Default: "SR";
 *     allowed values:
 *       - "SM"   Smallest magnitude
 *       - "LM"   Largest magnitude
 *       - "SR"   Smallest real
 *       - "LR"   Largest real
 *       - "SI"   Smallest imaginary
 *                (only for complex scalar types)
 *       - "LI"   Largest imaginary
 *                (only for complex scalar types)
 *       - "BE"   Both ends
 *   - tolerance: Tolerance for eigensolver. Default: Default numeric
 *                tolerance (as in Constants.hh)
 *   - mode:      Arpack mode to use. We use mode 1 for normal
 *                eigenproblems and mode 2 for generalised
 *                eigenproblems by default (see below)
 *   - n_arnoldi_vectors: The number of Arnoldi vectors to use.
 *                Has to be larger than twice the number of eigenpairs
 *                to be obtained.
 *
 * ## The expected matrices in A, B and Diag.
 * ### mode 1
 * Normal solve mode. Only allowed if A == Diag and B is not specified
 * (not a generalised eigenproblem)
 *
 * ### mode 2
 * Generalised solve mode by taking the inverse of B.
 * Diag == $B^{-1} A$.
 *
 * ### mode 3
 * Shift-and-invert mode. Diag == $(A - \sigma B)^{-1}$
 *
 * ### modes > 3
 * Not supported. Check Arpack documentation for dnaupd,
 * dsaupd or znaupd on your own.
 *
 *
 * ## Handlers and events
 * Since we have no way of knowing when Arpack starts a new
 * iteration the functions start_iteration_step and end_iteration_step
 * are not used by this class. Consequently the handlers
 * before_iteration_step() and after_iteration_step() are never
 * triggered.
 *
 * \note Currently only double precision scalar types can be used.
 *
 * \tparam Eigenproblem  The eigenproblem to solve.
 * \tparam State         The state type of the solver.
 */
template <typename Eigenproblem,
          typename State = ArpackEigensolverState<Eigenproblem>>
class ArpackEigensolver : public EigensolverBase<State> {
    static_assert(
          std::is_same<Eigenproblem, typename State::eproblem_type>::value,
          "The type Eigenproblem and the implicit eigenproblem type in the SCF "
          "state have to agree");

    static_assert(
          std::is_same<typename Eigenproblem::scalar_type, double>::value,
          "Arpack can only solve real problems at double precision at "
          "the moment");

    // TODO remove assertion and generalise
    static_assert(Eigenproblem::hermitian,
                  "Can only solve Hermitian eigenproblems at the moment.");

    // TODO remove assertion and generalise
    static_assert(!Eigenproblem::generalised,
                  "Can only solve non-general eigenproblems at the moment.");

    // TODO remove assertion and generalise
    static_assert(std::is_same<typename Eigenproblem::matrix_a_type,
                               typename Eigenproblem::matrix_diag_type>::value,
                  "The type of the A matrix and the diag matrix needs to agree "
                  "at the moment.");

  public:
    //@{
    /** Forwarded types */
    typedef EigensolverBase<State> base_type;
    typedef typename base_type::state_type state_type;
    typedef typename base_type::scalar_type scalar_type;
    typedef typename base_type::real_type real_type;
    typedef typename base_type::evalue_type evalue_type;
    typedef typename base_type::evector_type evector_type;
    typedef typename base_type::esoln_type esoln_type;
    typedef typename base_type::stored_matrix_type stored_matrix_type;
    //@}

    /** \name Constructor */
    //@{
    /** Construct an eigensolver with the default parameters */
    ArpackEigensolver() {}

    /** Construct an eigensolver setting the parameters from the map */
    ArpackEigensolver(const krims::ParameterMap& map) : ArpackEigensolver() {
        update_control_params(map);
    }
    //@}

    /** \name Iteration control */
    ///@{
    /** \briefArpack mode used.
     *
     * See Arpack documentation for details what is expected for the
     * structure of the A and the Diag matrix in this case.
     *
     * \note We use 1 for the normal eigenproblem and 2 for the
     * geneneralised eigenproblem by default.
     */
    int mode = Eigenproblem::generalised ? 2 : 1;

    /** \name Number of arnoldi vectors
     *  By default 0, which implies that we use
     *  std::min(dim, 3*n_ep/2) vectors, where dim is the dimensionality
     *  of the problem and n_ep is the number of eigenpairs.
     */
    size_t n_arnoldi_vectors = 0;

    /** Maximum number of iterations */
    size_t max_iter = 100;

    /** Update control parameters from Parameter map */
    void update_control_params(const krims::ParameterMap& map) {
        base_type::update_control_params(map);
        max_iter = map.at(ArpackEigensolverKeys::max_iter, max_iter);
        n_arnoldi_vectors = map.at(ArpackEigensolverKeys::n_arnoldi_vectors,
                                   n_arnoldi_vectors);
        mode = map.at(ArpackEigensolverKeys::mode, mode);
    }
    ///@}

    /** Implementation of the IterativeSolver method */
    void solve_state(state_type& state) const override;

  private:
    /** Assert that the state of the control parameters is sensible.
     *  In case its not, raise an ExcInvalidEigensolverParameters
     *  exception */
    void assert_valid_control_params(state_type& s) const;

    /** Deal with the ido parameter we got */
    void arpack_ido_step(state_type& s, std::array<int, 14> ipntr) const;
};

//
// ----------------------------------------------------------
//

template <typename Eigenproblem, typename State>
void ArpackEigensolver<Eigenproblem, State>::assert_valid_control_params(
      state_type& state) const {
    // TODO I do not like this, since some checks could be performed
    // before the call of solve ... separate this?
    //
    // Some stuff is more general and affects multiple solver classes
    // this is not reflected here

    const Eigenproblem& problem = state.eigenproblem();

    //
    // which
    //
    const std::string& which = base_type::which;
    if (krims::IsComplexNumber<typename State::evalue_type>::value) {
        solver_assert(which == "SR" || which == "LR" || which == "SM" ||
                            which == "LM" || which == "SI" || which == "LI",
                      state, ExcInvalidSolverParametersEncountered(
                                   "The value " + which +
                                   " for which is not allowed in an "
                                   "Arpack solver call that will yield "
                                   "complex eigenvalues"));
    } else {
        solver_assert(which == "SR" || which == "LR" || which == "SM" ||
                            which == "LM" || which == "BE",
                      state, ExcInvalidSolverParametersEncountered(
                                   "The value " + which +
                                   " for which is not allowed in an "
                                   "Arpack solver call that will yield "
                                   "real eigenvalues"));
    }

    //
    // mode
    //
    if (Eigenproblem::generalised) {
        solver_assert(
              mode != 1, state,
              ExcInvalidSolverParametersEncountered(
                    "For generalised eigenproblems mode 1 is not allowed."));
    } else {
        solver_assert(mode != 2, state,
                      ExcInvalidSolverParametersEncountered(
                            "For normal eigenproblems mode 2 is not allowed."));
    }
    // dsaupd accepts modes between 1 and 5.
    // dnaupd accepts modes between 1 and 4.
    // znaupd accepts modes between 1 and 3.
    solver_assert(
          mode >= 1 && mode <= 5, state,
          ExcInvalidSolverParametersEncountered(
                "The value " + std::to_string(mode) +
                " for mode is not allowed. Only values within [1,5] are ok."));

    if (mode == 1) {
        solver_assert(&problem.A() == &problem.Diag(), state,
                      ExcInvalidSolverParametersEncountered(
                            "For mode 1 the matrices A and Diag need "
                            "to be the same objects."));
    }

    //
    // Arnoldi vectors
    //
    if (n_arnoldi_vectors > 0) {
        // if == 0 we select automatically.
        solver_assert(n_arnoldi_vectors <= problem.dim(), state,
                      ExcInvalidSolverParametersEncountered(
                            "The number of arnoldi vectors " +
                            std::to_string(n_arnoldi_vectors) +
                            "  should be no larger than the problem size " +
                            std::to_string(problem.dim()) + "."));

        solver_assert(n_arnoldi_vectors > 2 * problem.n_ep() + 1, state,
                      ExcInvalidSolverParametersEncountered(
                            "The number of arnoldi vectors (== " +
                            std::to_string(n_arnoldi_vectors) +
                            " is too small to obtain " +
                            std::to_string(problem.n_ep()) + " eigenvalues."));
    }

    //
    // Eigenpairs
    //
    solver_assert(
          problem.n_ep() < problem.dim(), state,
          ExcInvalidSolverParametersEncountered(
                "The number of eigenpairs to compute (==" +
                std::to_string(problem.n_ep()) +
                ") must be less than the dimensionality of the problem (== " +
                std::to_string(problem.dim()) + ")"));
}

template <typename Eigenproblem, typename State>
void ArpackEigensolver<Eigenproblem, State>::arpack_ido_step(
      state_type& state, std::array<int, 14> ipntr) const {
    const Eigenproblem& problem = state.eigenproblem();
    typedef typename state_type::scalar_type scalar_type;

    // Nothing to do if we are converged.
    if (state.ido == 99) return;

    typedef PtrVector<scalar_type> ptrvec_t;
    // Build MultiVector<PtrVector>s for the important vectors, such
    // that we can use apply with them.
    // Note: Since Arpack uses 1-based indices (Fortran) we need to
    // take one away before we use them.
    scalar_type* workd = &(*state.workd_ptr)[0];
    auto x = make_as_multivector<const ptrvec_t>(workd + ipntr[0] - 1,
                                                 problem.dim());
    auto y = make_as_multivector<ptrvec_t>(workd + ipntr[1] - 1, problem.dim());
    auto Bx = make_as_multivector<const ptrvec_t>(workd + ipntr[2] - 1,
                                                  problem.dim());

    switch (state.ido) {
        case -1:
            // Arpack wants  y = Diag*x and Bx is not valid.
            if (mode == 3) {
                // special, since our definition
                // of the Operator Diag deviates from
                // the one Arpack uses.
                std::unique_ptr<scalar_type[]> tmp_ptr(
                      new scalar_type[problem.dim()]);
                auto tmp = make_as_multivector<ptrvec_t>(tmp_ptr.get(),
                                                         problem.dim());
                problem.B().apply(x, tmp);  // tmp = B x
                // y = (A - \sigma B)^{-1} tmp
                problem.Diag().apply(tmp, y);
            } else {
                problem.Diag().apply(x, y);  // y = Diag * x
            }
            break;
        case 1:
            // Arpack wants  y = Diag*x and Bx is valid.
            if (mode == 3) {
                // special, see above
                // y = (A - \sigma B)^{-1} * B * x
                problem.Diag().apply(Bx, y);
                // TODO Implement apply like this:
                // https://trilinos.org/docs/r12.8/packages/tpetra/doc/html/classTpetra_1_1Operator.html#a900b02363414fa0decad11d466c320ce
            } else {
                problem.Diag().apply(x, y);  // y = Diag * x
            }
            break;
        case 2:
            problem.B().apply(x, y);  // y = B x
            break;
        case 3:
            // TODO ido==3 exists, but we do not have it implemented
            assert_dbg(false, krims::ExcNotImplemented());
            break;
        default:
            // Check that we have a sensible ido
            assert_dbg(false, ExcArpackInvalidIdo(state.ido));
    }
}

template <typename Eigenproblem, typename State>
void ArpackEigensolver<Eigenproblem, State>::solve_state(
      state_type& state) const {
    // TODO Remove assertions and generalise.
    assert_dbg(mode == 1, krims::ExcNotImplemented());

    // -------

    const Eigenproblem& problem = state.eigenproblem();
    esoln_type& soln = state.eigensolution();

    // Assert that control parameters and state fit.
    assert_valid_control_params(state);

    // Setup Arpack wrapper
    size_t n_arnoldi_actual = n_arnoldi_vectors;
    if (n_arnoldi_actual == 0) {
        // Auto-determine number of arnoldi vectors:
        n_arnoldi_actual = std::min(problem.dim(), 3 * problem.n_ep() / 2);
        if (n_arnoldi_actual < 2) n_arnoldi_actual = 2;
    }

    static_assert(
          std::is_same<typename Eigenproblem::scalar_type, double>::value,
          "Arpack can only solve real problems at double precision at "
          "the moment");

    detail::ds_upd_wrapper arpack{Eigenproblem::generalised, problem.dim(),
                                  base_type::which,          problem.n_ep(),
                                  base_type::tolerance,      n_arnoldi_actual,
                                  state.resid_ptr,           state.workd_ptr};

    // Setup iparam array:
    // TODO In theory one *could* expose this first feature to the interface
    state.iparam[0] = 1;  // Automatically determine shifts
    state.iparam[2] = static_cast<int>(max_iter);
    state.iparam[6] = mode;

    // The info parameter. If different from 0 on first call
    // a residual from a previous run is reused.
    int info = state.have_old_residual ? 1 : 0;
    state.have_old_residual = true;

    // start_iteration_step and end_iteration_step are
    // unused in this implementation, since we do not
    // really know when an iteration starts.
    while (state.ido != 99) {
        // Perform an Arnoldi/Lanczos step in Arpack
        // and then deal with the ido until Arpack
        // flags convergence.
        arpack.arnoldi_step(state.ido, state.iparam, info);
        arpack_ido_step(state, arpack.ipntr);
    }

    // info == 0 implies that all is fine
    // info == 1 implies that we cannot find any more Ritz vectors
    //           (but we still can get some of them)
    solver_assert(info == 0 || info == 1, state, ExcArpackInfo("dsaupd", info));

    // TODO The case of info == 1 has never been tested.
    assert_sufficiently_tested(info != 1);

    //
    // Compute eigenpairs
    //
    //! Container for eigenvectors, one after another
    std::vector<double> evectors;
    //! We currently use no shift (mode 3 not implemented)
    const double sigma = 0.0;
    int info_ev;
    arpack.eigenpairs(sigma, state.iparam, soln.evalues(), evectors, info_ev);
    solver_assert(info_ev == 0, state, ExcArpackInfo("dseupd", info_ev));

    size_t converged_ep = problem.n_ep();
    if (info == 1) {
        // Shrink to keep only those eigenpairs which are converged.
        converged_ep = state.iparam[4];
        soln.evalues().resize(converged_ep);
    }

    assert_dbg(converged_ep * problem.dim() <= evectors.size(),
               krims::ExcInternalError());

    soln.evectors().reserve(converged_ep);
    for (double* begin = evectors.data();
         begin < evectors.data() + converged_ep * problem.dim();
         begin += problem.dim()) {
        evector_type v(begin, begin + problem.dim());
        soln.evectors().push_back(std::move(v));
    }

    // info == 1 implies that we cannot find any more Ritz vectors
    solver_assert(info != 1, state,
                  ExcArpackCannotFindMoreEigenpairs(problem.n_ep(),
                                                    soln.evalues().size()));
}

}  // namespace linalgwrap
#endif  // LINALGWRAP_HAVE_ARPACK
