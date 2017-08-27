## ---------------------------------------------------------------------
##
## Copyright (C) 2016-17 by the lazyten authors
##
## This file is part of lazyten.
##
## lazyten is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## lazyten is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with lazyten. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

## DO NOT EDIT
## This file is automatically generated from a file in the repository "krims".
## Edit the original and call the script "update_from_sister_repos.sh" instead.

# Try to find a cmake module which gets shipped with krims.
# This module is not intended to be used from within krims at all.
# It is just for convenience of downstream libraries, such that they can
# find krims during their install process.
#
# This module is closely related to the krims cmake module
# FindPackageAutocheckoutFallback in the sense that the two use largely the same
# options and the mechanism of finding the krims module are extremly similar.
# This should be taken into account when updating one of the two modules.
#

option(AUTOCHECKOUT_MISSING_REPOS "Automatically checkout missing repositories" OFF)
option(AUTOCHECKOUT_FORCED
	"Force autocheckout of repositories, even if they are present on the system. \
Implies AUTOCHECKOUT_MISSING_REPOS set to ON" OFF)

if (NOT DEFINED EXTERNAL_DIR)
	if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../external/get_krims.sh")
		set(EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/../external")
	else()
		message(FATAL_ERROR "Could not determine the location of the external \
projects. Try to specify it manually using the variable EXTERNAL_DIR.")
	endif()
endif()

function(autocheckout_krims)
	# We could not include it with the hint => try autocheckout
	execute_process(
		COMMAND "sh" "get_krims.sh"
		WORKING_DIRECTORY "${EXTERNAL_DIR}"
		RESULT_VARIABLE RES
	)
	if (NOT RES EQUAL 0)
		message(FATAL_ERROR "Getting krims failed with error: ${RES}")
	endif()
endfunction()


# NOTE: This has to be a macro otherwise it opens a new scope, which means
#       that the variables set in the included modules are *not* available
#       to the outside scope!
macro(include_krims_cmake_module MODULE)
	# Search for a cmake module distributed with krims in the following places:
	#   - The hint given via 'krims_DIR' environment or cmake cache variable
	#   - The location generated by inspecting the CMAKE_INSTALL_PREFIX
	#   - The default include call (i.e. system modules)
	#   - Do and autocheckout (if allowed) and search for the module in the
	#     checked out version of krims.
	#
	# If AUTOCHECKOUT_FORCED is ON only the last search (i.e. autocheckout
	# and a search in the automatically checked out version) will be done.
	#

	if(NOT EXISTS "${EXTERNAL_DIR}/get_krims.sh")
		message(FATAL_ERROR "get_krims.sh does not exist in EXTERNAL_DIR \
== ${EXTERNAL_DIR}.")
	endif()

	# Determine FIRST_TRY_LOCATION, i.e the place where we first try to find
	# the module
	set(FIRST_TRY_LOCATION "")
	if (NOT "$ENV{krims_DIR}" STREQUAL ""
			AND NOT "$ENV{krims_DIR}" MATCHES "NOTFOUND"
			AND NOT "$ENV{krims_DIR}" MATCHES "AUTOCHECKOUT")
		set(FIRST_TRY_LOCATION
			"$ENV{krims_DIR}/share/cmake/modules/${MODULE}.cmake")
	endif()

	# krims_DIR points to the directory containing the package config
	# this is a sister directory to the modules directory containing
	# the module files we are after.
	if (NOT "${krims_DIR}" STREQUAL ""
			AND NOT "${krims_DIR}" MATCHES "NOTFOUND"
			AND NOT "${krims_DIR}" MATCHES "AUTOCHECKOUT")
		set(FIRST_TRY_LOCATION "${krims_DIR}/../modules/${MODULE}.cmake")
	endif()

	# Krims is already configured as a project somewhere ...
	# use that location with highest priority and do not proceed to autocheckout,
	# even if that is forced. The rationale is that otherwise autocheckout will be
	# forced in each submodule ... which is not what we want.
	if (NOT "${krims_SOURCE_DIR}" STREQUAL "")
		set(FIRST_TRY_LOCATION "${krims_SOURCE_DIR}/cmake/modules/${MODULE}.cmake")
	elseif("${krims_DIR}" STREQUAL "krims_DIR-AUTOCHECKOUT"
			OR "$ENV{krims_DIR}" STREQUAL "krims_DIR-AUTOCHECKOUT"
			OR AUTOCHECKOUT_FORCED)
		# Do autocheckout if it is forced or otherwise wanted
		# and if we have not configured krims elsewhere.
		autocheckout_krims()

		set(FIRST_TRY_LOCATION
			"${EXTERNAL_DIR}/krims/cmake/modules/${MODULE}.cmake")
	endif()

	# Try the first try location
	set(RES "NOTFOUND")
	if (NOT "${FIRST_TRY_LOCATION}" STREQUAL "")
		include("${FIRST_TRY_LOCATION}" OPTIONAL RESULT_VARIABLE RES)
	endif()
	unset(FIRST_TRY_LOCATION)

	if (AUTOCHECKOUT_FORCED AND "${RES}" STREQUAL "NOTFOUND")
		# We should have found it now.
		message(FATAL_ERROR "Even though autocheckout is forced, we could not \
find the cmake module ${MODULE} in the krims module checked out. \
Check that you have the correct version of krims checked out.")
	endif()

	# Try the install location
	if ("${RES}" STREQUAL "NOTFOUND")
		include("${CMAKE_INSTALL_PREFIX}/share/cmake/modules/${MODULE}.cmake"
			OPTIONAL RESULT_VARIABLE RES)
	endif()

	# Try a normal include
	if ("${RES}" STREQUAL "NOTFOUND")
		include(${MODULE} OPTIONAL RESULT_VARIABLE RES)
	endif()

	if ("${RES}" STREQUAL "NOTFOUND")
		# We still could not find it => autocheckout
		if (AUTOCHECKOUT_MISSING_REPOS)
			autocheckout_krims()
			include("${EXTERNAL_DIR}/krims/cmake/modules/${MODULE}.cmake")
		else()
			message(FATAL_ERROR "Could not find the ${MODULE} module, \
which should be distributed with krims. You can either provide the installation prefix \
of the krims library in the environment variable 'krims_DIR' or enable autocheckout
via '-DAUTOCHECKOUT_MISSING_REPOS=ON'.")
		endif()
	endif()

endmacro(include_krims_cmake_module)
