#ifndef __MB_TASK_H__
#define __MB_TASK_H__

#include <random>
#include <functional>
#include <system/Math.hpp>

namespace mb {
    class Task {
    private:
        bool mTimed { false};
        bool mComplete { false };
        bool mEveryTick { true };
        uint8_t mPadding { 0 };
        float mCurrent { 0.0f };
        float mDuration { 1.0f };
        uint32_t mTaskId { 0 };

        std::function<void(Task*,float)> mFunc;
        Task* mNext { nullptr };
        Task* mPrev { nullptr };

        Task(){}
        ~Task(){}

    public:
        float GetRuntime() { return mCurrent; }
        void Complete() { mComplete = true; }
        static Task* New(std::function<void(Task*, float)>, bool timed=false, float duration=0.0f, bool everyTick=true);
        static void Free(Task*);
        static void Initialize(std::size_t);
        static void Update(float);
        static void Cleanup();
        static bool ActiveTasks();
    };
}

#endif
