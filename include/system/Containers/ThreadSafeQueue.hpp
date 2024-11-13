#ifndef __MB_TSQ_H__
#define __MB_TSQ_H__
#include <thread>
#include <queue>

namespace mb {

template<typename T>
class TSQ {
private:
    std::mutex mLock;
    std::queue<T> mElements;

public:
    bool empty() { return mElements.empty(); }

    void pop(T& to){
        std::scoped_lock(mLock);
        to = mElements.front();
        mElements.pop();
    }

    void push(T e){
        std::scoped_lock(mLock);
        mElements.push(std::move(e));
    }

    TSQ(){}
    ~TSQ(){}
};

}

#endif