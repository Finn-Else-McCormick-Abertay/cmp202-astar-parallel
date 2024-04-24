#include "TimeStatistics.h"
#include "queue"

using namespace std::chrono;

TimeStatistics::TimeStatistics(const std::vector<TimeCompound>& times) : m_times(times) {}

const std::vector<TimeCompound>& TimeStatistics::times() const { return m_times; }

TimeCompound TimeStatistics::mean() const {
	nanoseconds sumTime = nanoseconds(0);
	for (auto& time : m_times) { sumTime += time.asNanosecondsFull(); }

	nanoseconds meanTime = sumTime / m_times.size();
	return TimeCompound(meanTime);
}

TimeCompound TimeStatistics::standardDeviation() const {
	nanoseconds meanTime = mean().asNanosecondsFull();
	double sumSquareDifference = 0.0;
	for (auto& time : m_times) { sumSquareDifference += pow(static_cast<double>((time.asNanosecondsFull() - meanTime).count()), 2); }
	double standardDeviation = sqrt(sumSquareDifference / static_cast<double>(m_times.size()));
	return TimeCompound(nanoseconds(static_cast<long long int>(standardDeviation)));
}

TimeCompound TimeStatistics::median() const {
	std::priority_queue<nanoseconds> queue;
	for (auto& time : m_times) { queue.push(time.asNanosecondsFull()); }
	std::vector<nanoseconds> orderedTimes;
	while (!queue.empty()) { orderedTimes.push_back(queue.top()); queue.pop(); }
	
	auto numValues = orderedTimes.size();
	if (numValues % 2 == 0) {
		nanoseconds below = orderedTimes.at(numValues / 2), above = orderedTimes.at((numValues / 2) + 1);
		return TimeCompound(below + ((above - below) / 2));
	}
	else { return TimeCompound(orderedTimes.at((numValues / 2) + 1)); }
}