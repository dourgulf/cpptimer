//
//  systemtime.cpp
//  cpptimer
//
//  Created by dawenhing on 2018/01/11.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "SystemTime.hpp"
#include <climits>

#if defined(_MSC_VER) && _MSC_VER < 1700
#include <intrin.h>
#define CompilerMemBar() _ReadWriteBarrier()
#else
#include <atomic>
#define CompilerMemBar() std::atomic_signal_fence(std::memory_order_seq_cst)
#endif

#if defined(SYS_WINDOWS)
#include <sys/timeb.h>
#include <windows.h>

namespace cpptimer
{
	void sleep(int milliseconds)
	{
		::Sleep(milliseconds);
	}

	SystemTime getSystemTime()
	{
		LARGE_INTEGER t;
		CompilerMemBar();
		if (!QueryPerformanceCounter(&t)) {
			return static_cast<SystemTime>(-1);
		}
		CompilerMemBar();

		return static_cast<SystemTime>(t.QuadPart);
	}

	double getTimeDelta(SystemTime start)
	{
		LARGE_INTEGER t;
		CompilerMemBar();
		if (start == static_cast<SystemTime>(-1) || !QueryPerformanceCounter(&t)) {
			return -1;
		}
		CompilerMemBar();

		auto now = static_cast<SystemTime>(t.QuadPart);

		LARGE_INTEGER f;
		if (!QueryPerformanceFrequency(&f)) {
			return -1;
		}

		return static_cast<double>(static_cast<__int64>(now - start)) / f.QuadPart * 1000;
	}
}

#elif defined(SYS_APPLE)

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
#include <time.h>

namespace cpptimer
{

	void sleep(int milliseconds)
	{
		::usleep(milliseconds * 1000);
	}

	SystemTime getSystemTime()
	{
		CompilerMemBar();
		std::uint64_t result = mach_absolute_time();
		CompilerMemBar();

		return result;
	}

	double getTimeDelta(SystemTime start)
	{
		CompilerMemBar();
		std::uint64_t end = mach_absolute_time();
		CompilerMemBar();

		mach_timebase_info_data_t tb = { 0 };
		mach_timebase_info(&tb);
		double toNano = static_cast<double>(tb.numer) / tb.denom;

		return static_cast<double>(end - start) * toNano * 0.000001;
	}
}

#elif defined(SYS_NIX)

#include <unistd.h>

namespace cpptimer
{

	void sleep(int milliseconds)
	{
		::usleep(milliseconds * 1000);
	}

	SystemTime getSystemTime()
	{
		timespec t;
		CompilerMemBar();
		if (clock_gettime(CLOCK_MONOTONIC_RAW, &t) != 0) {
			t.tv_sec = (time_t)-1;
			t.tv_nsec = -1;
		}
		CompilerMemBar();

		return t;
	}

	double getTimeDelta(SystemTime start)
	{
		timespec t;
		CompilerMemBar();
		if ((start.tv_sec == (time_t)-1 && start.tv_nsec == -1) || clock_gettime(CLOCK_MONOTONIC_RAW, &t) != 0) {
			return -1;
		}
		CompilerMemBar();

		return static_cast<double>(static_cast<long>(t.tv_sec) - static_cast<long>(start.tv_sec)) * 1000 + double(t.tv_nsec - start.tv_nsec) / 1000000;
	}
}
#endif

namespace cpptimer
{
	SystemTime getCurrentMillisecs() {
#if defined(SYS_WINDOWS)
		_timeb timebuffer;
		_ftime(&timebuffer);
		unsigned long long ret = timebuffer.time;
		ret = ret * 1000 + timebuffer.millitm;
		return ret;
#else
		timeval tv;
		::gettimeofday(&tv, 0);
		longtime_t ret = tv.tv_sec;
		return ret * 1000 + tv.tv_usec / 1000;
#endif
		return cpptimer::getSystemTime();
	}

	void localtime(struct tm* const pTm, const time_t* const time)
	{
#if defined(SYS_WINDOWS)
		localtime_s(pTm, time);
#else
		localtime_r(time, pTm);
#endif
	}
}