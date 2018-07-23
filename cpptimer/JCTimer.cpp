//
//  Created by dawenhing on 10/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "JCTimer.hpp"

longtime_t TimerManager::getCurrentMillisecs() {
	return cpptimer::getCurrentMillisecs();
}

#ifdef TRACE_TIMER
#include <iostream>
#include <iomanip>
#include <sstream>
#include <strstream>
#include <chrono>
#include <stdarg.h>
#define TRACE(...) traceAction(__VA_ARGS__)
#else
#define TRACE(...) do{}while(0)
#endif

#ifdef TRACE_TIMER
Timer::~Timer() {
    TRACE("dtor");
}

void Timer::setTraceName(const std::string& name) {
    traceName_ = name;
};
void Timer::traceAction(const char* fmt, ...) {
    auto now = std::chrono::system_clock::now();
    auto nowt = std::chrono::system_clock::to_time_t(now);
    struct tm tmNow;
	cpptimer::localtime(&tmNow, &nowt);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::stringstream prefix;
    prefix << std::put_time(&tmNow, "%F-%T");
    prefix << "." << std::setw(4) << std::setfill('0') << milliseconds;
    prefix << " timer ";
    if (traceName_.length() > 0) {
        prefix << traceName_;
    }
    else {
        prefix << this;
    }
    char action[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(action, sizeof(action)-1, fmt, ap);    
    va_end(ap);
    
    fprintf(stdout, "%s %s\n", prefix.str().c_str(), action);
}
#endif

void Timer::runAfter(int ms, std::function<void ()> f) {
    cancel();
    repeat = false;
    interval_ = ms;
    expires_ = interval_ + TimerManager::getCurrentMillisecs();
    task_ = std::move(f);
    addToManager();
    TRACE("setup after %d(ms)", ms);
}

void Timer::runAt(longtime_t ts, std::function<void ()> f) {
    cancel();
    repeat = false;
    expires_ = ts;
    interval_ = static_cast<int>(ts - TimerManager::getCurrentMillisecs());
    task_ = std::move(f);
    addToManager();
    TRACE("setup at %lld", ts);        
}

void Timer::runEvery(int ms, std::function<void ()> f) {
    cancel();
    repeat = true;
    interval_ = ms;
    expires_ = interval_ + TimerManager::getCurrentMillisecs();
    task_ = std::move(f);
    addToManager();              
    TRACE("setup repeat %d(ms)", ms);        
}

void Timer::cancel() {
    if (isValid()) {
		removeFromManager();
		markInvalid();
        TRACE("cancel");
    }        
}
void Timer::onTimer(longtime_t now) {
    if (repeat) {
        expires_ = interval_ + now;
        addToManager();
    }
    else {
        markInvalid();
    }
    TRACE("ontimer");
    task_();        
}
