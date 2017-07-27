//
//  JCCascadeTimer.cpp
//  cpptimer
//
//  Created by dawenhing on 07/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "JCCascadeTimer.hpp"

#define TVN_BITS 6
#define TVR_BITS 8
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)
#define OFFSET(N) (TVR_SIZE + (N) *TVN_SIZE)
#define INDEX(V, N) ((V >> (TVR_BITS + (N) *TVN_BITS)) & TVN_MASK)

// TimerManager
JCCascadeTimerManager::JCCascadeTimerManager() {
    tvec_.resize(TVR_SIZE + 4 * TVN_SIZE);
    checkTime_ = getCurrentMillisecs();
}

JCCascadeTimerT JCCascadeTimerManager::createTimer() {
    return JCCascadeTimerT(new JCCascadeTimer(*this));
}

void JCCascadeTimerManager::addTimer(JCCascadeTimerT timer) {
    longtime_t expires = timer->expires();
    unsigned long long idx = expires - checkTime_;
    
    if (idx < TVR_SIZE) {
        timer->vecIndex_ = expires & TVR_MASK;
    }
    else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
        timer->vecIndex_ = OFFSET(0) + INDEX(expires, 0);
    }
    else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
        timer->vecIndex_ = OFFSET(1) + INDEX(expires, 1);
    }
    else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
        timer->vecIndex_ = OFFSET(2) + INDEX(expires, 2);
    }
    else if ((long long) idx < 0) {
        timer->vecIndex_ = checkTime_ & TVR_MASK;
    }
    else {   
        if (idx > 0xffffffffUL)
        {
            idx = 0xffffffffUL;
            expires = idx + checkTime_;
        }
        timer->vecIndex_ = OFFSET(3) + INDEX(expires, 3);
    }
    
    time_list_t &tlist = tvec_[timer->vecIndex_];
    tlist.push_back(timer);
    timer->tailling_ = tlist.end();
    --timer->tailling_;
}

void JCCascadeTimerManager::removeTimer(JCCascadeTimerT timer) {
    time_list_t& tlist = tvec_[timer->vecIndex_];
    tlist.erase(timer->tailling_);
}

void JCCascadeTimerManager::checkTimers()
{
    longtime_t now = getCurrentMillisecs();
    while (checkTime_ <= now) {
        int index = checkTime_ & TVR_MASK;
        if (!index &&
            !cascade(OFFSET(0), INDEX(checkTime_, 0)) &&
            !cascade(OFFSET(1), INDEX(checkTime_, 1)) &&
            !cascade(OFFSET(2), INDEX(checkTime_, 2)))
        {
            cascade(OFFSET(3), INDEX(checkTime_, 3));
        }
        ++checkTime_;
        
        time_list_t& tlist = tvec_[index];
        time_list_t temp;
        temp.splice(temp.end(), tlist);
        for (auto it = temp.begin(); it != temp.end(); ++it) {
            (*it)->onTimer(now);
        }
    }
}

int JCCascadeTimerManager::cascade(int offset, int index) {
    time_list_t& tlist = tvec_[offset + index];
    time_list_t temp;
    temp.splice(temp.end(), tlist);
    
    for (auto it = temp.begin(); it != temp.end(); ++it) {
        addTimer(*it);
    }
    return index;
}

