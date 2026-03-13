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
 #include <udjat/tools/system/info.h>
 #include <udjat/agent/process.h>

 namespace Udjat {

	const char * Process::Agent::fieldNames[] = {
		"rss",
		"VSize",
		"Shared"
	};

	Process::Agent::Field Process::Agent::get_field(const char *name) {

		for(size_t ix = 0; ix < N_ELEMENTS(fieldNames); ix++) {

			if(!strcasecmp(name,fieldNames[ix])) {
				return (Process::Agent::Field) ix;
			}

		}

		throw system_error(EINVAL, system_category(),"Invalid field name");
	}

	Process::Agent::Agent() {
	}

	Process::Agent::Agent(const pugi::xml_node &node) : Abstract::Agent(node) {
	}

	Process::Agent::~Agent() {
		Process::Controller::getInstance().remove(this);
	}

	bool Process::Agent::probe(const Identifier &ident) const noexcept {

		string exe;

		try {

			exe = ident.to_string();

		} catch(...) {

			return false;

		}

		return probe(exe.c_str());
	}

	Process::Identifier::State Process::Agent::process_state() const noexcept {

		if(proc) {
			return proc->state();
		}

		return Process::Identifier::Dead;

	}

	void Process::Agent::start() {
#ifdef DEBUG
		info() << "Starting states=" << states.size() << endl;
#endif // DEBUG
		Process::Controller::getInstance().insert(this);

		if(!proc) {
			updated(true);
		}

	}

	/*
	void Process::Agent::get(const Request &request, Response &response) {

		super::get(request,response);

		if(pid) {
			pid->get(response);
			Identifier::Stat(pid).get(response);
		} else {
			Identifier::Stat().get(response);
		}

	}
	*/

	void Process::Agent::set(const pid_t pid) {

		if(pid < 0) {
			set((Identifier *) nullptr);
		} else {
			set(Process::Controller::getInstance().find(pid));
		}

	}

	void Process::Agent::set(Identifier *proc) {

		if(proc == this->proc) {
			// No changes, just return.
			return;
		}

		this->proc = proc;

		// Notify state change.
		if(proc) {
			Logger::String{"Detected on pid '",((pid_t) *proc),"'"}.info(name());
		} else {
			Logger::String{"Not available"}.info(name());
		}

		// Mark as updated and changed.
		updated(true);
	}

	float Process::Agent::cpu_usage() const noexcept {

		if(proc) {
			return proc->cpu_usage();
		}

		return 0;
	}

	unsigned long long Process::Agent::rss() const {
		if(proc) {
			return Identifier::Stat(proc).rss();
		}
		return 0;
	}

	unsigned long long Process::Agent::vsize() const {
		if(proc) {
			return proc->vsize();
		}
		return 0;
	}

	unsigned long long Process::Agent::shared() const {
		if(!proc) {
			return proc->shared();
		}
		return 0;
	}

	unsigned long long Process::Agent::value(Field field) const {

		if(!proc) {
			return 0;
		}

		Identifier::Stat stat(proc);

		switch(field) {
		case Rss:
			return stat.rss();

		case VSize:
			return stat.vsize();

		case Shared:
			return stat.shared();

		default:
			throw runtime_error("Unexpected field id");
		}

	}

	float Process::Agent::percent(Field field) const {

		if(!proc) {
			return 0;
		}

		Identifier::Stat stat{proc};

		switch(field) {
		case Rss:


			// RSS - Return resident pages / totalram.
			{
				float value = (float) stat.rss();

				if(value > 0) {
					return  value / ((float) Udjat::System::Info().totalram);
				}

			}
			break;

		case VSize:

			// VSize - Return APP VSize / (totalram + totalswap)
			{
				Udjat::System::Info info;
				float value = (float) stat.vsize();

				if(value > 0) {
					return value / ((float) (info.totalram + info.totalswap));
				}
			}
			break;

		case Shared:

			// Shared  - Return APP Shared / totalshared
			{
				float value = (float) stat.shared();

				if(value > 0) {
					return  value / ((float) Udjat::System::Info().sharedram);
				}

			}
			break;

		default:
			throw runtime_error("Unexpected field id");
		}

		return 0;

	}
 }
