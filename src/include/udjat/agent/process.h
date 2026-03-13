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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/agent/state.h>
 #include <udjat/tools/process.h>
 #include <udjat/tools/xml.h>

 namespace Udjat {

	namespace Process {

 		class UDJAT_API Agent : public Udjat::Abstract::Agent {
		public:
			class State;

		private:
			friend class Process::Controller;

			/// @brief The process identifier for this agent.
			Identifier *proc = nullptr;

			/// @brief Agent states.
			std::vector<std::shared_ptr<State>> states;

		protected:
			Agent();
			Agent(const XML::Node &node);

			void start() override;

			/// @brief Test if the identifier exename match the agent.
			/// @brief exename The identifier exename.
			/// @return true if the identifier match the agent requirements.
			virtual bool probe(const char *exename) const noexcept;

			/// @brief Test if the identifier match the agent.
			/// @param ident A process identifier.
			/// @return true if the identifier match the agent requirements.
			virtual bool probe(const Identifier &ident) const noexcept;

			std::shared_ptr<Abstract::State> computeState() override;

			/// @brief Set process pid.
			void set(const pid_t pid);

			/// @brief Set process identifier.
			void set(Identifier *info);

		public:
			static std::shared_ptr<Udjat::Abstract::Agent> AgentFactory(const XML::Node &node);

			~Agent() override;

			inline const pid_t pid() const noexcept {
				return proc ? proc->pid() : (pid_t) -1;
			}

			inline const Identifier * process() const noexcept {
				return proc;
			}

			Value & getProperties(Value &value) const override;

			std::shared_ptr<Abstract::State> StateFactory(const XML::Node &node) override;

			float cpu_usage() const noexcept;

			/// @brief The size of memory that are currently resident in RAM in bytes.
			unsigned long long rss() const;

			/// @brief Virtual memory size in bytes.
			unsigned long long vsize() const;

			/// @brief The amount of resident memory that is shared with other processes.
			unsigned long long shared() const;

			/// @brief Field types.
			enum Field : uint8_t {
				Rss,	///< @brief The size of memory that are currently resident in RAM in bytes.
				VSize,	///< @brief Virtual memory size in bytes.
				Shared	///< @brief The amount of resident memory that is shared with other processes.
			};

			static const char * fieldNames[];
			static Field get_field(const char *name);

			/// @brief Get field value in bytes.
			unsigned long long value(Field field) const;

			/// @brief Get field value in % of the system total.
			float percent(Field field) const;

			Process::Identifier::State process_state() const noexcept;

 		};

 	}

 }

