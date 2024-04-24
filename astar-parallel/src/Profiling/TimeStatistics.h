#pragma once

#include "Timer.h"
#include <vector>

class TimeStatistics
{
public:
	TimeStatistics(const std::vector<TimeCompound>& times);

	const std::vector<TimeCompound>& times() const;

	TimeCompound mean() const;
	TimeCompound standardDeviation() const;
	TimeCompound median() const;

private:
	std::vector<TimeCompound> m_times;
};