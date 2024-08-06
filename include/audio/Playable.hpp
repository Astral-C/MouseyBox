#ifndef __PLAYABLE_H__
#define __PLAYABLE_H__

#include <filesystem>

namespace mb::Audio {
    class Playable {
    protected:
        bool mLoop;
    public:
        std::string mName;
        bool ShouldLoop() { return mLoop; }
        void ShouldLoop(bool l) { mLoop = l; }

        virtual void Loop() {  }
        virtual bool AtEnd() { return true; }
        virtual void Mix(uint8_t*, int) {}

        virtual void Load(std::filesystem::path) {}
        virtual void Load(uint8_t*, size_t) {}


        Playable();
        ~Playable();
    };
}


#endif