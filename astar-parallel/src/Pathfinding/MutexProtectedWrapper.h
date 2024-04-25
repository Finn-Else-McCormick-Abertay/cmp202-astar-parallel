#pragma once

#include <mutex>

// Simple wrapper for protecting a value with a mutex
template<typename T> class Protected
{
private:
	T m_value; std::mutex m_mutex;
public:
	Protected(T val) : m_value(val), m_mutex(std::mutex()) {}
	Protected(const Protected& other) : m_value(other.m_value), m_mutex(std::mutex()) {}

	T get() {
		auto lock = std::lock_guard(m_mutex);
		return m_value;
	}

	void set(T val) {
		auto lock = std::lock_guard(m_mutex);
		m_value = val;
	}
};