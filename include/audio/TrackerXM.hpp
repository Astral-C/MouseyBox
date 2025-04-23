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
#include <xm.h>

namespace mb::Audio {

    class TrackerXM : public Playable {
    private:
        xm_context_t* mXMHandle;
        uint16_t mLastPattern { 0 };
        uint16_t mLastPatternRows { 0 };
    public:

        void Loop();
        bool AtEnd();
        void Reset();
        
        void Mix(uint8_t*, uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, std::size_t);

        TrackerXM();
        ~TrackerXM();
    };
}


#endif

