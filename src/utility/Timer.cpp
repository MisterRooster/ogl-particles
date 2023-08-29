/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Timer.h"

using namespace std::chrono;


namespace nhahn
{
	Timer::Timer()
	{
		reset();
	}

	uint64_t Timer::getMilliseconds() const
	{
		auto newTime = m_HRClock.now();
		duration<double> dur = newTime - m_StartTime;

		return duration_cast<milliseconds>(dur).count();
	}

	uint64_t Timer::getMicroseconds() const
	{
		auto newTime = m_HRClock.now();
		duration<double> dur = newTime - m_StartTime;

		return duration_cast<microseconds>(dur).count();
	}

	uint64_t Timer::getStartMs() const
	{
		nanoseconds startTimeNs = m_StartTime.time_since_epoch();

		return duration_cast<milliseconds>(startTimeNs).count();
	}

	void Timer::reset()
	{
		m_StartTime = m_HRClock.now();
	}
}
