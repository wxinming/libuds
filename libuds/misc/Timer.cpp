#include "Timer.h"

uds::Timer::Timer()
{
	QueryPerformanceFrequency(&m_large);
	m_freq = static_cast<double>(m_large.QuadPart);
	getStartTime();
}

uds::Timer::~Timer()
{

}

void uds::Timer::getStartTime()
{
	QueryPerformanceCounter(&m_large);
	m_start = m_large.QuadPart;
}

double uds::Timer::getEndTime()
{
	QueryPerformanceCounter(&m_large);
	return static_cast<double>(m_large.QuadPart - m_start) * 1000 / m_freq;
}

void uds::Timer::sleep(double ms)
{
	getStartTime();
	while (getEndTime() < ms);
}
