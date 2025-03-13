#ifdef ENABLE_OPENMPT
#ifndef __OPENMPT_H__
#define __OPENMPT_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <SDL3/SDL.h>
#include <libopenmpt/libopenmpt.hpp>

namespace mb::Audio {

    class OpenMPT : public Playable {
    private:
        openmpt::module* mMod { nullptr };
        std::size_t mAtEnd;
        public:

        void Loop();
        bool AtEnd();
        void Reset();
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, std::size_t);

        OpenMPT();
        ~OpenMPT();
    };
}


#endif
#endif
