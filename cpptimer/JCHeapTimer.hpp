//
//  JCHeapTimer.hpp
//  cpptimer
//
//  Created by dawenhing on 07/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#ifndef JCHeapTimer_hpp
#define JCHeapTimer_hpp

#include "JCTimer.hpp"

#include <vector>
#include <iostream>
#include <memory>

class JCHeapTimer;

using JCHeapTimerT = std::shared_ptr<JCHeapTimer>;

class JCHeapTimerManager: JCTimerManager {
public:
    JCHeapTimerManager() {
        heap_.reserve(16);
    }
    JCHeapTimerT createTimer();
        
    void checkTimers() override;
    
private:
    friend class JCHeapTimer;
    void addTimer(JCHeapTimerT timer);
    void removeTimer(JCHeapTimerT timer);
    
    void upHeap(size_t index);
    void downHeap(size_t index);
    void swapHeap(size_t left, size_t right);
        
private:
    struct HeapEntry {
        longtime_t time;
        JCHeapTimerT timer;
    };
    std::vector<HeapEntry> heap_;
};

class JCHeapTimer: public JCTimer, public std::enable_shared_from_this<JCHeapTimer> {
private:    
    JCHeapTimer(JCHeapTimerManager &m): manager_(m), heapIndex_(-1){}
    
private:    
    bool isValid() override { return (heapIndex_ != -1); }
    void markInvalid() override { heapIndex_ = -1; }
    void addToManager() override { manager_.addTimer(shared_from_this()); }
    void removeFromManager() override { manager_.removeTimer(shared_from_this()); }
    
private:
    friend class JCHeapTimerManager;
    JCHeapTimerManager &manager_;  

    size_t heapIndex_;
};

#endif /* JCHeapTimer_hpp */
