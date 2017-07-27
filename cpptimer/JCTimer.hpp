//
//  JCTimer.hpp
//  cpptimer
//
//  Created by dawenhing on 10/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#ifndef JCTimerManager_hpp
#define JCTimerManager_hpp

#include <functional>
#include <string>

#define TRACE_TIMER 1

using longtime_t = unsigned long long;

class JCTimerManager {
public:
    virtual ~JCTimerManager() {};
    virtual void checkTimers() = 0;
    static longtime_t getCurrentMillisecs();
};

class JCTimer {
public:
    void runAfter(int ms, std::function<void ()> f);
    void runAt(longtime_t ts, std::function<void ()> f);    
    void runEvery(int ms, std::function<void ()> f);    
    void cancel();
    void onTimer(longtime_t now);
    inline longtime_t expires() { return expires_; }
    
#ifdef TRACE_TIMER
    virtual ~JCTimer();
    void setTraceName(const std::string& name);
    void traceAction(const char* fmt, ...);
#endif
private:
    virtual bool isValid() = 0;
    virtual void markInvalid() = 0;
    virtual void addToManager() = 0;
    virtual void removeFromManager() = 0;
        
private:
    bool repeat;
    int interval_;
    std::function<void(void)> task_;
    longtime_t expires_;
    
#ifdef TRACE_TIMER
    std::string traceName_;
#endif
};

#endif /* JCTimerManager_hpp */
