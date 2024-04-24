#include "Timer.h"

using namespace std::chrono;

TimeCompound::TimeCompound(const nanoseconds& time) : m_timeFull(time) {}
TimeCompound::TimeCompound(const TimeCompound& other) : m_timeFull(other.m_timeFull) {}
TimeCompound::TimeCompound(TimeCompound& other) : m_timeFull(other.m_timeFull) {}

double TimeCompound::asSecondsFull() const {
	return static_cast<double>(m_timeFull.count()) / static_cast<double>(std::nano::den);
}
nanoseconds TimeCompound::asNanosecondsFull() const { return m_timeFull; }

seconds TimeCompound::secondsComponent() const {
	return duration_cast<seconds>(m_timeFull);
}
milliseconds TimeCompound::millisecondsComponent() const {
	return duration_cast<milliseconds>(m_timeFull) - duration_cast<milliseconds>(duration_cast<seconds>(m_timeFull));
}
microseconds TimeCompound::microsecondsComponent() const {
	return duration_cast<microseconds>(m_timeFull) - duration_cast<microseconds>(duration_cast<milliseconds>(m_timeFull));
}
nanoseconds TimeCompound::nanosecondsComponent() const {
	return m_timeFull - duration_cast<nanoseconds>(duration_cast<microseconds>(m_timeFull));
}

std::ostream& operator<<(std::ostream& os, const TimeCompound& time) {
	return os << time.secondsComponent() << " " << time.millisecondsComponent() << " " << time.microsecondsComponent() << " " << time.nanosecondsComponent();
}



void Timer::start() {
	m_startTime = steady_clock::now();
	m_running = true;
}

void Timer::stop() {
	m_endTime = steady_clock::now();
	m_running = false;
}

TimeCompound Timer::elapsedTime() const { return TimeCompound(elapsedTimeNanoseconds()); }

nanoseconds Timer::elapsedTimeNanoseconds() const {
	time_point<steady_clock> workingEndTime = (m_running ? steady_clock::now() : m_endTime);
	return workingEndTime - m_startTime;
}