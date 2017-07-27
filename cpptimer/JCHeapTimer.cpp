//
//  JCHeapTimer.cpp
//  cpptimer
//
//  Created by dawenhing on 07/07/2017.
//  Copyright © 2017 dawenhing. All rights reserved.
//

#include "JCHeapTimer.hpp"

JCHeapTimerT JCHeapTimerManager::createTimer() {
    return JCHeapTimerT(new JCHeapTimer(*this));
}

void JCHeapTimerManager::addTimer(JCHeapTimerT timer) {
    timer->heapIndex_ = heap_.size();
    HeapEntry entry = { timer->expires(), timer };
    heap_.push_back(entry);
    upHeap(heap_.size() - 1);
}

void JCHeapTimerManager::removeTimer(JCHeapTimerT timer) {
    size_t index = timer->heapIndex_;
    if (!heap_.empty() && index < heap_.size()) {
        if (index == heap_.size() - 1) {
            heap_.pop_back();
        }
        else {
            swapHeap(index, heap_.size() - 1);
            heap_.pop_back();
            size_t parent = (index - 1) / 2;
            if (index > 0 && heap_[index].time < heap_[parent].time)
                upHeap(index);
            else
                downHeap(index);
        }
    }
}

void JCHeapTimerManager::checkTimers() {
    longtime_t now = getCurrentMillisecs();
    while (!heap_.empty() && heap_[0].time <= now) {
        JCHeapTimerT timer = heap_[0].timer;
        removeTimer(timer);
        timer->onTimer(now);
    }
}

void JCHeapTimerManager::upHeap(size_t index) {
    size_t parent = (index - 1) / 2;
    while (index > 0 && heap_[index].time < heap_[parent].time) {
        swapHeap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void JCHeapTimerManager::downHeap(size_t index) {
    size_t child = index * 2 + 1;
    while (child < heap_.size()) {
        size_t minChild = (child + 1 == heap_.size() || heap_[child].time < heap_[child + 1].time) ? child : child + 1;
        if (heap_[index].time <= heap_[minChild].time) {
            break;
        }
        swapHeap(index, minChild);
        index = minChild;
        child = index * 2 + 1;
    }
}

void JCHeapTimerManager::swapHeap(size_t left, size_t right) {
    std::swap(heap_[left].timer, heap_[right].timer);
    std::swap(heap_[left].time, heap_[right].time);

    heap_[left].timer->heapIndex_ = left;
    heap_[right].timer->heapIndex_ = right;
}
