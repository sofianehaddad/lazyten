#!/bin/bash
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

. update_from_sisters.lib.sh || exit 1

update_file "krims" ".clang-format" || exit 1
update_file "krims" ".clang-tidy" || exit 1
update_file "krims" "cmake/findCatch.cmake" || exit 1
update_file "krims" "cmake/findRapidcheck.cmake" || exit 1
update_file "krims" "cmake/IncludeKrimsCmakeModule.cmake" || exit 1
update_file "krims" "doc/Doxyfile.in" || exit 1
update_file "krims" "external/get.lib.sh" || exit 1
update_file "krims" "external/get_rapidcheck.sh" || exit 1
update_file "krims" "templates/README.md" || exit 1
