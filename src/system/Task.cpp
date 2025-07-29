#include <map>
#include <system/Task.hpp>

namespace mb {
    uint32_t mTaskMax { 0 };
    Task* mTasks { nullptr };
    Task* mFreeHead { nullptr };
    Task* mUsedHead { nullptr };

    void Task::Initialize(std::size_t taskMax){
        if(mTasks != nullptr) return;
        mTaskMax = taskMax;
        mTasks = new Task[mTaskMax];
        mFreeHead = mTasks;

        Task* prev = nullptr;
        for(std::size_t t = 0; t < mTaskMax; t++){
            Task* tsk = &mTasks[t];
            tsk->mTaskId = t;
            
            tsk->mNext = &mTasks[t + 1];
            tsk->mPrev = prev;
            prev = tsk;
        }

        mTasks[mTaskMax - 1].mNext = nullptr;
    }

    void Task::Cleanup(){
        if(mTasks != nullptr){
            delete[] mTasks;
        }
        mTasks = nullptr;
    }

    Task* Task::New(std::function<void(Task*, float)> task, bool timed, float duration, bool everyTick){
        Task* t = mFreeHead;
        if(mFreeHead == nullptr) return nullptr;
        
        mFreeHead = mFreeHead->mNext;
        
        if(mUsedHead == nullptr){
            // Used-list is empty, init used-list with this entity as the current head
            mUsedHead = t;
            t->mPrev = nullptr;
            t->mNext = nullptr;
        } else {
            // Used-list is not empty, push to front of current used list
            mUsedHead->mPrev = t;
            t->mNext = mUsedHead;
            t->mPrev = nullptr;
            mUsedHead = t;
        }

        t->mFunc = task;
        t->mTimed = timed;
        t->mDuration = duration;
        t->mEveryTick = everyTick;

        return t;
    }

    void Task::Free(Task* t){
        if(t < mTasks || t > mTasks + mTaskMax) return;

        // Remove from used-list
        if(t->mPrev != nullptr){
            t->mPrev->mNext = t->mNext;
        }

        if(t->mNext != nullptr){
            t->mNext->mPrev = t->mPrev;
        }

        if(t == mUsedHead){
            mUsedHead = t->mNext;
        }

        t->mPrev = nullptr;

        t->mNext = mFreeHead;
        mFreeHead->mPrev = t;
        mFreeHead = t;
    }
    
    void Task::Update(float dt){
        for(Task* task = mUsedHead; task != nullptr;){
            Task* next = task->mNext;
            if(task->mTimed){
                if(task->mEveryTick) task->mFunc(task, dt);
                task->mCurrent += dt*10.0f;
                if(task->mTimed && task->mCurrent >= task->mDuration){
                    if(!task->mEveryTick) task->mFunc(task, dt);
                    Task::Free(task);
                }
            } else if(!task->mTimed){
                task->mFunc(task, dt);
                if(task->mComplete){
                    Task::Free(task);
                }
            }
            task = next;
        }
    }
}