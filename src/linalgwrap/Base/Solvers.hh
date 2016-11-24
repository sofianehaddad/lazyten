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
/** \file Includes the core interfaces and datastructures used
 *  by the iterative and direct solvers provided by the
 *  external libraries */

// Base solver
#include "Solvers/SolverBase.hh"
#include "Solvers/SolverExceptions.hh"
#include "Solvers/SolverStateBase.hh"

// Iterative methods base types and structs
#include "Solvers/IterativeSolver.hh"
#include "Solvers/IterativeSolverKeys.hh"
#include "Solvers/IterativeSolverState.hh"

// Eigensolvers base types and structs
#include "Solvers/Eigenproblem.hh"
#include "Solvers/Eigensolution.hh"
#include "Solvers/EigensolverBase.hh"
#include "Solvers/EigensolverBaseKeys.hh"
#include "Solvers/EigensolverStateBase.hh"