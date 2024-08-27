#ifndef __OGG_H__
#define __OGG_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <SDL.h>
#define STB_VORBIS_HEADER_ONLY
#include <audio/stb_vorbis.h>

namespace mb::Audio {

    class Ogg : public Playable {
    private:
        stb_vorbis* mVorbisHandle;
    public:

        void Loop();
        bool AtEnd();
        void Reset();
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        Ogg();
        ~Ogg();
    };
}


#endif