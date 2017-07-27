//
//  JCTimer.cpp
//  cpptimer
//
//  Created by dawenhing on 10/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "JCTimer.hpp"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif

longtime_t JCTimerManager::getCurrentMillisecs() {
#ifdef _MSC_VER
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
}

#ifdef TRACE_TIMER
#include <iostream>
#include <iomanip>
#include <sstream>
#include <strstream>
#define TRACE(...) traceAction(__VA_ARGS__)
#else
#define TRACE(...) do{}while(0)
#endif

#ifdef TRACE_TIMER
JCTimer::~JCTimer() {
    TRACE("dtor");
}

void JCTimer::setTraceName(const std::string& name) {
    traceName_ = name;
};
void JCTimer::traceAction(const char* fmt, ...) {
    auto now = std::chrono::system_clock::now();
    auto nowt = std::chrono::system_clock::to_time_t(now);
    struct tm tmNow;
    localtime_r(&nowt, &tmNow);
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

void JCTimer::runAfter(int ms, std::function<void ()> f) {
    cancel();
    repeat = false;
    interval_ = ms;
    expires_ = interval_ + JCTimerManager::getCurrentMillisecs();
    task_ = std::move(f);
    addToManager();
    TRACE("setup after %d(ms)", ms);
}

void JCTimer::runAt(longtime_t ts, std::function<void ()> f) {
    cancel();
    repeat = false;
    expires_ = ts;
    interval_ = static_cast<int>(ts - JCTimerManager::getCurrentMillisecs());
    task_ = std::move(f);
    addToManager();
    TRACE("setup at %lld", ts);        
}

void JCTimer::runEvery(int ms, std::function<void ()> f) {
    cancel();
    repeat = true;
    interval_ = ms;
    expires_ = interval_ + JCTimerManager::getCurrentMillisecs();
    task_ = std::move(f);
    addToManager();              
    TRACE("setup repeat %d(ms)", ms);        
}

void JCTimer::cancel() {
    if (isValid()) {
        markInvalid();
        removeFromManager();
        TRACE("cancel");
    }        
}
void JCTimer::onTimer(longtime_t now) {
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
