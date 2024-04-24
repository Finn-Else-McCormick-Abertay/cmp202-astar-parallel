#include "Profiler.h"

Profiler::Profiler(int numIterations) : m_numIterations(numIterations) {}

TimeStatistics Profiler::timingResults() const { return TimeStatistics(m_timingResults); }


ProfilerBlocking::ProfilerBlocking(int numIterations) : Profiler(numIterations) {}


ProfilerNonBlocking::ProfilerNonBlocking(int numIterations) : Profiler(numIterations), m_inProgressSemaphore(0) { m_inProgressSemaphore.release(); }

bool ProfilerNonBlocking::isFinished() {
	if (m_inProgressSemaphore.try_acquire()) {
		m_inProgressSemaphore.release();
		return true;
	}
	return false;
}