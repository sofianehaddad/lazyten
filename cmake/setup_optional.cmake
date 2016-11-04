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

# adds entries to these things
#
#       LINALGWRAP_DEPENDENCIES			everyone needs these libraries
#       LINALGWRAP_DEPENDENCIES_DEBUG		debug mode needs these extras
#       LINALGWRAP_DEPENDENCIES_RELEASE		release mode needs these extras
#       LINALGWRAP_DEPENDENCIES_TEST		tests need these extra libraries
#
#       LINALGWRAP_DEFINITIONS			definitions for all compilation
#       LINALGWRAP_DEFINITIONS_DEBUG		definitions for debug mode
#       LINALGWRAP_DEFINITIONS_RELEASE		definitions for release mode
#

####################
#-- C++ standard --#
####################
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 14)
	message(STATUS "Detected C++14 support: Setting LINALGWRAP_HAVE_CXX14")
	LIST(APPEND LINALGWRAP_DEFINITIONS "LINALGWRAP_HAVE_CXX14")
endif()
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 17)
	message(STATUS "Detected C++17 support: Setting LINALGWRAP_HAVE_CXX17")
	LIST(APPEND LINALGWRAP_DEFINITIONS "LINALGWRAP_HAVE_CXX17")
endif()

################
#--  ARPACK  --#
################
SET(ARPACK_DIR "" CACHE PATH "An optional hint to an ARPACK installation")
find_library(
	ARPACK_LIBRARY
	NAMES arpack
	HINTS ${ARPACK_DIR}
	PATH_SUFFIXES lib64 lib
)
if(NOT ${ARPACK_LIBRARY} MATCHES "-NOTFOUND")
	message(STATUS "Found ARPACK at ${ARPACK_LIBRARY}")
	LIST(APPEND LINALGWRAP_DEFINITIONS "LINALGWRAP_HAVE_ARPACK")
	set(LINALGWRAP_DEPENDENCIES ${LINALGWRAP_DEPENDENCIES} ${ARPACK_LIBRARY})
endif()

