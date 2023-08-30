/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <chrono>


namespace nhahn
{
	/** Timer class used for querying high precision timers. */
	class Timer
    {
    public:
		/** Construct the timer and start timing. */
		Timer();

		/** Returns time in milliseconds since timer was initialized or last reset. */
		uint64_t getMilliseconds() const;
		/** Returns time in microseconds since timer was initialized or last reset. */
		uint64_t getMicroseconds() const;
		/**
		 * Returns the time at which the timer was initialized, in milliseconds.
		 *
		 * @return	Time in milliseconds.
		 */
		uint64_t getStartMs() const;

		/** Reset the timer to zero. */
		void reset();

	private:
		std::chrono::steady_clock m_HRClock;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    };
}
