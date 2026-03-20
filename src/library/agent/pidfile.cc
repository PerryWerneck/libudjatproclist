/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
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
 #include <private/agent.h>
 #include <private/controller.h>
 #include <iostream>
 #include <fstream>

 #ifdef HAVE_UNISTD_H
	 #include <unistd.h>
 #endif // HAVE_UNISTD_H

 namespace Udjat {

	Process::PidFileAgent::PidFileAgent(const char *filename, const pugi::xml_node &node) : Process::Agent(node), pidfile(filename) {
	}

	bool Process::PidFileAgent::probe(const char UDJAT_UNUSED(*exename)) const noexcept {
		return false;
	}

	void Process::PidFileAgent::start() {
		refresh();
	}

	bool Process::PidFileAgent::refresh() {

		if(!process()) {

			// Check for pidfile.
			if(access(pidfile,R_OK) == 0) {

				ifstream pf;
				pf.open(pidfile);

				// Found pidfile, read it!
				unsigned int pid = 0;
				pf >> pid;
				pf.close();

				if(pid) {
					Logger::String{"Got '", pid, "' from '", pidfile, "'"}.info(name());
					set((pid_t) pid);
				}

			}

		}

		return Process::Agent::refresh();

	}

 }

