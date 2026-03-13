/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
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
 #include <udjat/module/abstract.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/url.h>
 #include <udjat/module/process.h>
 #include <udjat/agent/process.h>

 namespace Udjat {

	Udjat::Module * Process::Module::Factory(const char *name) {
		return new Process::Module(name);
	}

	Process::Module::Module(const char *name, const char *description) : Udjat::Module{name,description}, Udjat::Abstract::Agent::Factory{"process"} {
	}

	Process::Module::~Module() {
	}

	std::shared_ptr<Abstract::Agent> Process::Module::AgentFactory(const XML::Node &node) const {
		return Process::Agent::AgentFactory(node);
	}

 }

