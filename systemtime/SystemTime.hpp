//
//  systemtime.hpp
//  cpptimer
//
//  Created by dawenhing on 2018/01/11.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#ifndef SYSTEMTIME_H__
#define SYSTEMTIME_H__


#if defined(_WIN32)
#define SYS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#define SYS_APPLE
#elif defined(__linux__) || defined(__FreeBSD__) || defined(BSD)
#define SYS_NIX
#else
#error "Unknown platform"
#endif

#if defined(SYS_WINDOWS)
#include <time.h>
namespace cpptimer { typedef unsigned long long SystemTime; }
#elif defined(SYS_APPLE)
#include <cstdint>
namespace cpptimer { typedef std::uint64_t SystemTime; }
#elif defined(SYS_NIX)
#include <time.h>
namespace cpptimer { typedef timespec SystemTime; }
#endif

namespace cpptimer
{
	void sleep(int milliseconds);

	SystemTime getSystemTime();

	// Returns the delta time, in milliseconds
	double getTimeDelta(SystemTime start);

	void localtime(struct tm* const pTm, const time_t* const time);

	SystemTime getCurrentMillisecs();

}

#endif