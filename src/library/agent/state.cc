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
 #include <udjat/agent/state.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/properties.h>
 #include <udjat/tools/converters.h>
 #include <udjat/tools/string.h>
 #include <cfloat>
 #include <climits>

 using Pid = Udjat::Process::Identifier;

 namespace Udjat {

	/// @brief Agent state based on process state.
	class ProcessState : public Process::Agent::State {
	private:
		Process::Identifier state = Pid::Undefined;

	public:
		ProcessState(const char *s, const Properties &props) : Process::Agent::State(props), state(Process::Identifier::StateFactory(s)) {
		}

		bool test(const Process::Agent &agent) const noexcept override {
			debug("State=",agent.process_state()," mine=",state);
			return agent.process_state() == state;
		}

	};

	/// @brief Agent state based on process availability.
	class ProcessAvailable : public Process::Agent::State {
	private:
		bool required;

	public:
		ProcessAvailable(bool r, const Properties &props) : Process::Agent::State(props), required(r) {
		}

		bool test(const Process::Agent &agent) const noexcept override {
			auto state = agent.process_state();
			bool available = (state != Pid::Dead && state != Pid::DeadCompat && state != Pid::Zombie && state != Pid::Stopped);
			debug("State=",agent.process_state()," available=",(available ? "yes" : "no"));
			return available == required;
		}

	};

	/// @brief State by process usage in %
	class ProcessUsage : public Process::Agent::State {
	protected:
		float from, to;

		bool compare(float value) const noexcept {
			return value >= from && value <= to;
		}

	public:
		ProcessUsage(const Properties &props) : Process::Agent::State(props) {
			if(props.contains("from")) {
				from = props.get("from",0.0f);
			} else {
				from = props.get("value",0.0f);
			}	
			if(props.contains("to")) {
				to = props.get("to",FLT_MAX);
			} else {
				to = props.get("value",FLT_MAX);
			}
		}

	};

	std::shared_ptr<Abstract::State> Process::Agent::StateFactory(const Properties &props) {

		std::shared_ptr<State> state;

		// Agent state by process
		if(props.contains("process-state")) {

			const auto attr = props.get("process-state");

			if(strcasecmp(attr.c_str(),"available") == 0) {
				state = make_shared<ProcessAvailable>(true,props);
			} else if(strcasecmp(attr.c_str(),"not-available") == 0) {
				state = make_shared<ProcessAvailable>(false,props);
			} else {
				state = make_shared<ProcessState>(attr.c_str(),props);
			}

			if(state) {
				states.push_back(state);
				return state;
			}
		}

		if(props.contains("field-name")) {
			Process::Agent::Field field = Process::Agent::get_field(props.get("field-name").c_str());

			/// @brief State based on field value.
			class Value : public Process::Agent::State {
			private:
				Process::Agent::Field field;
				unsigned long long from = 0;
				unsigned long long to = 0;

			public:
				Value(Process::Agent::Field f, const Properties &props) : Process::Agent::State(props), field(f) {
					if(props.contains("from")) {
						from = props.get("from").as_ull();
					} else {
						from = props.get("value").as_ull();
					}
					if(props.contains("to")) {
						to = props.get("to").as_ull();
					} else {
						to = props.get("value").as_ull(ULLONG_MAX);
					}
				}

				bool test(const Process::Agent &agent) const noexcept override {
					unsigned long long value = agent.value(field);
					return value >= from && value <= to;
				}

			};

			return make_shared<Value>(field, props);

		}

		return state;
	}

	std::shared_ptr<Abstract::State> Process::Agent::computeState() {

		for(auto state : this->states) {
			if(state->test(*this))
				return state;
		}

		debug("Using default state");
		return Abstract::Agent::computeState();

	}

 }
