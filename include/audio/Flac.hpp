#ifndef __OGG_H__
#define __OGG_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <SDL3/SDL.h>
#include <audio/formats/dr_flac.h>

namespace mb::Audio {

    class Flac : public Playable {
    private:
        drflac* mFlacHandle { nullptr };
    public:

        void Loop();
        bool AtEnd();
        void Reset();
        
        void Mix(uint8_t*, uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        Flac();
        ~Flac();
    };
}


#endif