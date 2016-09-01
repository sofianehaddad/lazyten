## ---------------------------------------------------------------------
##
## Copyright (C) 2016 by the linalgwrap authors
##
## This file is part of linalgwrap.
##
## linalgwrap is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## linalgwrap is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with linalgwrap. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

# Finds and sets up rapidcheck under the target name stored in
#     rapidcheck_TARGET
# such that just linking against it as a dependency does everything 
# automatically.
#
# In case the rapidcheck library is not found and AUTOCHECKOUT_MISSING_LIBS is set to
# on, rapidcheck is automatically checked out and built.
# Otherwise a fatal error is produced.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_REPOS "Automatically checkout missing repositories" OFF)

#
# -------
#

# Unset building rapidcheck in this scope
set(BUILD_EXTERNAL_RAPIDCHECK off)

set(rapidcheck_TARGET "rapidcheck")
if (TARGET "${rapidcheck_TARGET}")
	message(STATUS "Found target rapidcheck, assume rapidcheck already configured for build.")
	return()
endif()

# Try to find rapidcheck somewhere
find_package(rapidcheck QUIET CONFIG)
string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)
if ("${rapidcheck_DIR}" STREQUAL "rapidcheck_DIR-NOTFOUND")
	if (AUTOCHECKOUT_MISSING_REPOS)
		execute_process(
			COMMAND "sh" "get_rapidcheck.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external"
			RESULT_VARIABLE RES
		)
		if (NOT RES EQUAL 0)
			message(FATAL_ERROR "Getting rapidcheck from git failed with error: ${RES}")
		endif()

		# Print message and return
		set(BUILD_EXTERNAL_RAPIDCHECK on)
		return()
	endif()

	message(FATAL_ERROR "Could not find rapidcheck library.
Either disable testing of ${PROJECT_NAME} by setting ${PROJECT_UPPER}_ENABLE_TESTS to OFF \
or enable autocheckout via -DAUTOCHECKOUT_MISSING_REPOS=ON.")
endif()

message(WARNING "This part of findRapidcheck has never been tested.")

# Add library
set(rapidcheck_TARGET "Upstream::rapidcheck")
message(STATUS "Found rapidcheck config at ${rapidcheck_CONFIG}")
