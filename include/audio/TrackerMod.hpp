#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <tt/include/tracker.h>

namespace mb::Audio::Mod {
    

    class Tracker : public Playable {
    private:
        ModTracker mModTracker;

    public:

        void Loop() {  }
        bool AtEnd() { return false; }
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        Tracker();
        ~Tracker();
    };
}


#endif