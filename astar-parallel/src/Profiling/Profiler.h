#pragma once

#include <vector>
#include <functional>
#include <thread>
#include <semaphore>

#include "Timer.h"
#include "TimeStatistics.h"

class Profiler
{
private:
	int m_numIterations;
	std::vector<TimeCompound> m_timingResults;

protected:
	Profiler(int numIterations);

	template <typename R, typename ...A, typename ...PassedArgs>
	void profile(const std::function<R(A...)>& func, PassedArgs... args) {
		// First call to function always takes slightly longer, presumbaly due to some overhead in how std::function
		// accesses the function pointer, so we call it once before profiling to stop this affecting our results.
		func(args...);

		Timer timer;
		m_timingResults.clear(); m_timingResults.reserve(m_numIterations);
		
		for (int iteration = 0; iteration < m_numIterations; ++iteration) {
			timer.start();
			func(args...);
			timer.stop();
			m_timingResults.emplace_back(timer.elapsedTime());
		}
	}

public:
	virtual ~Profiler() = default;

	TimeStatistics timingResults() const;
};


// Version launched in the main thread
class ProfilerBlocking : public Profiler
{
public:
	ProfilerBlocking(int numIterations);

	template <typename R, typename ...A, typename ...PassedArgs>
	void performProfiling(const std::function<R(A...)>& func, PassedArgs... args) {
		profile(func, args...);
	}
};


// Version launched from a detached thread
class ProfilerNonBlocking : public Profiler
{
private:
	std::binary_semaphore m_inProgressSemaphore;

public:
	ProfilerNonBlocking(int numIterations);

	template <typename R, typename ...A, typename ...PassedArgs>
	void startProfiling(const std::function<R(A...)>& func, PassedArgs... args) {

		auto threadFunction =
		[&]<typename ...ThreadArgs>(ThreadArgs... threadArgs) {
			profile(func, threadArgs...);
			m_inProgressSemaphore.release();
		};

		m_inProgressSemaphore.acquire();
		std::thread launchThread(threadFunction, args...);
		launchThread.detach();
	}

	bool isFinished();
};