#pragma once

#include <chrono>
#include <iostream>

class TimeCompound
{
public:
	TimeCompound(const std::chrono::nanoseconds&);
	TimeCompound(const TimeCompound&);
	TimeCompound(TimeCompound&);

	double asSecondsFull() const;
	std::chrono::nanoseconds asNanosecondsFull() const;

	std::chrono::seconds secondsComponent() const;
	std::chrono::milliseconds millisecondsComponent() const;
	std::chrono::microseconds microsecondsComponent() const;
	std::chrono::nanoseconds nanosecondsComponent() const;

private:
	std::chrono::nanoseconds m_timeFull;
};

std::ostream& operator<<(std::ostream&, const TimeCompound&);


class Timer
{
public:
	void start();
	void stop();

	TimeCompound elapsedTime() const;

private:
	std::chrono::time_point<std::chrono::steady_clock> m_startTime, m_endTime;
	bool m_running = false;

	std::chrono::nanoseconds elapsedTimeNanoseconds() const;
};