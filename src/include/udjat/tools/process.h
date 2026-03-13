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
 #include <cstdint>
 #include <list>
 #include <mutex>
 #include <string>

 namespace Udjat {

 	namespace Process {

 		class Controller;

 		/// @brief Process identifier.
		/// @brief A single process.
		class UDJAT_API Identifier {
		public:

			/// @brief Process states.
			enum State : uint8_t {
				Running			= 'R',	///< @brief Running.
				Sleeping		= 'S',	///< @brief Sleeping in an interruptible wait.
				Waiting			= 'D',	///< @brief Waiting in uninterruptible disk sleep.
				Zombie			= 'Z',	///< @brief Zombie.
				Stopped			= 'T',	///< @brief Stopped (on a signal) or (before Linux 2.6.33) trace stopped.
				TracingStop		= 't',	///< @brief Tracing stop (Linux 2.6.33 onward).
				Paging			= 'W',	///< @brief Paging (only before Linux 2.6.0).
				Dead			= 'X',	///< @brief Dead (from Linux 2.6.0 onward).
				DeadCompat		= 'x',	///< @brief Dead (Linux 2.6.33 to 3.13 only).
				Wakekill		= 'K',	///< @brief Wakekill (Linux 2.6.33 to 3.13 only).
				Waking			= 'W',	///< @brief Waking (Linux 2.6.33 to 3.13 only).
				Parked			= 'P',	///< @brief Parked (Linux 3.9 to 3.13 only).

				Undefined		= 0		///< @brief Undefined.
			};

			static const struct StateName {
				State state;
				const char *name;
			} statenames[];

			static const StateName & StateNameFactory(const State state);
			static State StateFactory(const char *name);

		private:

			friend class Controller;

			pid_t procpid = -1;

			/// @brief Mutex for serialization.
			static std::recursive_mutex guard;

			//
			// Note:
			//
			// Required data per process:
			//
			// CPU Use in % of total.		OK
			// Memory Use in % of total.
			// Swap use in % of total.
			//

			/// @brief CPU Usage in %.
			struct {
				float percent = 0;			///< @brief CPU usage on last check.
				unsigned long last = 0;		///< @brief utime+stime got in the last refresh.
			} cpu;

			/// @brief Current state
			State current_state = (State) -1;

			/// @brief Set current state
			void set(const State state);

			/// @brief Cleanup states.
			void reset();

		public:
			constexpr Identifier(pid_t p) : procpid{p} {
			}

			~Identifier();

			Udjat::Value & getProperties(Udjat::Value &value) const;

			/// @brief Data from /proc/pid/stat.
			class UDJAT_API Stat {
			private:
				struct Properties {
					// https://www.kernel.org/doc/html/latest/filesystems/proc.html
					// https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat

					char 				state = 'X';		///< @brief Process state.
					int 				ppid = 0;			///< @brief The PID of the parent of this process.
					int 				pgrp = 0;			///< @brief The process group ID of the process.
					int 				session = 0;		///< @brief The session ID of the process.
					int 				tty_nr = 0;			///< @brief The controlling terminal of the process.  (The minor
					int 				tpgid = 0;			///< @brief The ID of the foreground process group of the
					unsigned int 		flags = 0;			///< @brief The kernel flags word of the process.  For bit
					unsigned long 		minflt = 0;			///< @brief The number of minor faults the process has made which have not required loading a memory page from disk.
					unsigned long 		cminflt = 0;		///< @brief The number of minor faults that the process's waited-for children have made.
					unsigned long 		majflt = 0;			///< @brief The number of major faults the process has made which have required loading a memory page from disk.
					unsigned long 		cmajflt = 0;		///< @brief The number of major faults that the process's waited-for children have made.
					unsigned long 		utime = 0;			///< @brief Amount of time that this process has been scheduled.
					unsigned long 		stime = 0;			///< @brief Amount of time that this process has been scheduled
					long				cutime = 0;			///< @brief Amount of time that this process's waited-for children have been scheduled in user mode
					long				cstime = 0;			///< @brief Amount of time that this process's waited-for children have been scheduled in kernel mode
					long				priority = 0;		///< @brief (Explanation for Linux 2.6) For processes running a
					long				nice = 0;			///< @brief The nice value (see setpriority(2)), a value in the
					long				num_threads = 0;	///< @brief Number of threads in this process (since Linux 2.6).
					long 				itrealvalue = 0;	///< @brief The time in jiffies before the next SIGALRM is sent
					unsigned long long	starttime = 0;		///< @brief The time the process started after system boot.  In
					unsigned long 		vsize = 0;			///< @brief Virtual memory size in bytes.
					long				rss = 0;			///< @brief Resident Set Size: Number of pages that are currently resident in RAM.
					unsigned long 		rsslim = 0;			///< @brief Current soft limit in bytes on the rss of the
					unsigned long 		startcode = 0;		///< @brief The address above which program text can run.
					unsigned long 		endcode = 0;		///< @brief The address below which program text can run.
					unsigned long 		startstack = 0;		///< @brief The address of the start (i.e., bottom) of the stack.
					unsigned long 		kstkesp = 0;		///< @brief The current value of ESP (stack pointer), as found
					unsigned long 		kstkeip = 0;		///< @brief The current EIP (instruction pointer).
					unsigned long 		signal = 0;			///< @brief The bitmap of pending signals, displayed as a
					unsigned long 		blocked = 0;		///< @brief The bitmap of blocked signals, displayed as a
					unsigned long 		sigignore = 0;		///< @brief The bitmap of ignored signals, displayed as a
					unsigned long 		sigcatch = 0;		///< @brief The bitmap of caught signals, displayed as a decimal
					unsigned long 		wchan = 0;			///< @brief This is the "channel" in which the process is
					unsigned long 		nswap = 0;			///< @brief Number of pages swapped (not maintained).
					unsigned long 		cnswap = 0;			///< @brief Cumulative nswap for child processes (not maintained).
					int 				exit_signal = 0;	///< @brief (since Linux 2.1.22) Signal to be sent to parent when we die.
					int 				processor = 0;		///< @brief (since Linux 2.2.8) CPU number last executed on.
					unsigned int		rt_priority = 0;	///< @brief (since Linux 2.5.19) Real-time scheduling priority, a number in the range
					unsigned int		policy = 0;			///< @brief (since Linux 2.5.19) Scheduling policy (see sched_setscheduler(2)).
					unsigned long long	blkio_ticks = 0;	///< @brief (since Linux 2.6.18) Aggregated block I/O delays, measured in clock ticks (centiseconds).
					unsigned long		guest_time = 0;		///< @brief (since Linux 2.6.24) Guest time of the process (time spent running a
					long 				cguest_time = 0;	///< @brief (since Linux 2.6.24)	Guest time of the process's children, measured in
					unsigned long		start_data = 0;		///< @brief (since Linux 3.3) Address above which program initialized and
					unsigned long		end_data = 0;		///< @brief (since Linux 3.3) Address below which program initialized and
					unsigned long		start_brk = 0;		///< @brief (since Linux 3.3) Address above which program heap can be expanded
					unsigned long		arg_start = 0;		///< @brief (since Linux 3.5) Address above which program command-line arguments
					unsigned long		arg_end = 0;		///< @brief (since Linux 3.5) Address below program command-line arguments (argv)
					unsigned long		env_start = 0;		///< @brief (since Linux 3.5) Address above which program environment is placed.
					unsigned long		env_end = 0;		///< @brief (since Linux 3.5) Address below which program environment is placed.
					int					exit_code = 0;		///< @brief (since Linux 3.5) The thread's exit status in the form reported by waitpid(2).
				
				} properties;

			public:

				Stat(pid_t pid);
				Stat(const Identifier *info);

				Udjat::Value & getProperties(Udjat::Value &value) const;

				static unsigned long long pgsize();

				/// @brief The amount of resident memory that is shared with other processes.
				unsigned long long shared() const;

				/// @brief The amount of memory currently resident in RAM
				unsigned long long rss() const;

				inline const char state() const noexcept {
					return properties.state;
				}

				inline unsigned long vsize() const noexcept {
					return properties.vsize;
				}
				
				inline unsigned long utime() const noexcept {
					return properties.utime;
				}

				inline unsigned long stime() const noexcept {
					return properties.stime;
				}
			};

			constexpr bool operator==(const pid_t pid) const {
				return this->procpid == pid;
			}

			constexpr bool operator==(const Identifier &entry) const {
				return this->procpid == entry.procpid;
			}

			constexpr bool operator==(const State &state) const {
				return this->current_state == state;
			}

			inline operator pid_t() const {
				return this->procpid;
			}

			inline pid_t pid() const noexcept {
				return this->procpid;
			}

			/// @brief Get the process name.
			/// @return String containing the process name.
			std::string to_string() const;

			State state();

			inline operator State() {
				return this->current_state;
			}

			/// @brief Get CPU usage in %.
			float cpu_usage() const {
				return (this->cpu.percent * 100);
			}

			inline unsigned long vsize() const {
				return Stat{*this}.vsize();
			}

			inline unsigned long shared() const {
				return Stat{*this}.shared();
			}
			
		};


 	}

 }
