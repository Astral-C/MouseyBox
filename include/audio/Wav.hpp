#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <SDL3/SDL.h>

namespace mb::Audio {

    class Wav : public Playable {
    private:
        SDL_AudioSpec mWavSpec;
        uint8_t* mData;
        uint32_t mDataLen;
        int mSampleOffset { 0 };
    public:

        void Loop();
        bool AtEnd();
        void Reset();
        
        void Mix(uint8_t*, uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        Wav();
        ~Wav();
    };
}


#endif