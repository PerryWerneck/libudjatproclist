/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2025 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/testsuite.h>
 #include <string>
 #include <udjat/tools/process.h>
 #include <udjat/tools/variant.h>

 #ifdef HAVE_UNISTD_H
 #include <unistd.h>
 #endif

 using namespace Udjat;
 using namespace std;

 #ifdef DEBUG 

 UDJAT_API void udjat_register_tests(Udjat::TestSuite &suite) noexcept {

	using Case = TestSuite::Case;

	suite.add(
		PACKAGE_DESCRIPTION,
		Case {
			"Test Process identifier",
			[](std::ostream &out) {			
				
				Process::Identifier proc{getpid()};

				Udjat::Variant props;
				proc.get_properties(props);
				props.to_yaml(out);

				return "Process identifier ok";
			}
		}
	);

 }  

 #endif


