//
//  main.cpp
//  cpptimer
//
//  Created by dawenhing on 27/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "JCTimer.hpp"
#include "JCHeapTimer.hpp"
#include "JCCascadeTimer.hpp"

#include <iostream>
#include <thread>
#include <iomanip>
#include <sstream>
#include <strstream>
#include <stdarg.h>

volatile bool stopped = false;

std::mutex consoleMutex;
void output(const char *fmt, ...) {
    
    auto now = std::chrono::system_clock::now();
    auto nowt = std::chrono::system_clock::to_time_t(now);
    struct tm tmNow;
    localtime_r(&nowt, &tmNow);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::stringstream timestamp;
    timestamp << std::put_time(&tmNow, "%F-%T");
    timestamp << "." << std::setw(4) << std::setfill('0') << milliseconds;

    std::lock_guard<std::mutex> lock(consoleMutex);
    fprintf(stdout, "%s ", timestamp.str().c_str());
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
}

using TestTimerManager = JCCascadeTimerManager;
using TestTimer = JCCascadeTimer;

std::unique_ptr<TestTimerManager> timerManager;
void runTimerManager() {
    timerManager = std::unique_ptr<TestTimerManager>(new TestTimerManager);
    auto timerThread = std::thread([]{
        while (!stopped) {
            timerManager->checkTimers();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
    timerThread.detach();
}

void testTimers() {
    output("testing");
    std::shared_ptr<TestTimer> cancelTimer;
    for(int i = 0; i<1024; i++) {
        auto timer = timerManager->createTimer();
        char name[64];
        snprintf(name, sizeof(name)-1, "timer%d", i);
        timer->setTraceName(name);
        timer->runAfter(1000 + i*2, []{
//            output("repeat timer running:");
        });
        if (i == 100) {
            cancelTimer = timer;
        }
    }
    cancelTimer->cancel();
    
    auto stopTimer = timerManager->createTimer();
    stopTimer->setTraceName("[stop timer]");
    stopTimer->runAfter(5 * 1000, []{
//        output("stop");
        stopped = true;
    });
}

int main(int argc, const char * argv[]) {
    runTimerManager();
    testTimers();
    std::thread waitFinished([]{
        while(!stopped) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    waitFinished.join();
    return 0;
}
