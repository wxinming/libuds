#pragma once
#include <Windows.h>
#include "Export.h"

namespace uds {
	class UDS_DLL_EXPORT Timer
	{
	public:
		Timer();
		~Timer();
		void getStartTime();
		double getEndTime();
		void sleep(double ms);

	private:
		double m_freq = 0.0f;
		long long m_start = 0L;
		LARGE_INTEGER m_large = { 0 };
	};
}

