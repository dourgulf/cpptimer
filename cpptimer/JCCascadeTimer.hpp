//
//  JCCascadeTimer.hpp
//  cpptimer
//
//  Created by dawenhing on 07/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#ifndef JCCascadeTimer_hpp
#define JCCascadeTimer_hpp

#include "JCTimer.hpp"

#include <list>
#include <vector>

class JCCascadeTimer;

using JCCascadeTimerT = std::shared_ptr<JCCascadeTimer>;

class JCCascadeTimerManager: JCTimerManager
{
public:
    JCCascadeTimerManager();
    
    JCCascadeTimerT createTimer();

    void checkTimers() override;
    
private:
    void addTimer(JCCascadeTimerT timer);
    void removeTimer(JCCascadeTimerT timer);
    int cascade(int offset, int index);
    
private:
    friend class JCCascadeTimer;

    typedef std::list<JCCascadeTimerT> time_list_t;
    std::vector<time_list_t> tvec_;
    longtime_t checkTime_;
};

class JCCascadeTimer: public JCTimer, public std::enable_shared_from_this<JCCascadeTimer>
{
public:
    JCCascadeTimer(JCCascadeTimerManager &m): manager_(m), vecIndex_(-1){}
    
    bool isValid() override { return (vecIndex_ != -1); }
    void markInvalid() override { vecIndex_ = -1; }
    void addToManager() override { manager_.addTimer(shared_from_this()); }
    void removeFromManager() override { manager_.removeTimer(shared_from_this()); }
    
private:    
    friend class JCCascadeTimerManager;
    JCCascadeTimerManager &manager_;  

    int vecIndex_;
    JCCascadeTimerManager::time_list_t::iterator tailling_;    
};

#endif /* JCCascadeTimer_hpp */
