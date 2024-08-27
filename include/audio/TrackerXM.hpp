#ifndef __XM_H__
#define __XM_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>

namespace mb::Audio {

    class XMTracker : public Playable {
    private:
        std::vector<Channel> mChannels;

    public:

        void Loop() {  }
        bool AtEnd() { return false; }
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        XMTracker();
        ~XMTracker();
    };
}


#endif