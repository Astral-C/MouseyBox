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
    T pop(T e){
        std::scoped_lock(mLock);
        return mElements.pop();
    }

    void push(T e){
        std::scoped_lock(mLock);
        mElements.push(e);
    }

    TSQ(){}
    ~TSQ(){}
};

}

#endif