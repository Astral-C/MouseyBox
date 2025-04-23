#ifndef __PLAYABLE_H__
#define __PLAYABLE_H__

#include <SDL3/SDL.h>
#include <filesystem>

namespace mb::Audio {
    class Playable {
    protected:
        bool mLoop;
        float mVolume { 1.0f };
        SDL_AudioSpec* mTargetSpec;
    public:
        std::string mName;
        bool ShouldLoop() { return mLoop; }
        void ShouldLoop(bool l) { mLoop = l; }
        void SetTargetSpec(SDL_AudioSpec* spec){ mTargetSpec = spec; }
        void Reset(){}

        virtual void Loop() {  }
        virtual bool AtEnd() { return true; }
        virtual void Mix(uint8_t*, uint8_t*, int) {}

        virtual void Load(std::filesystem::path) {}
        virtual void Load(uint8_t*, std::size_t) {}


        Playable();
        ~Playable();
    };
}


#endif