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

    class S3MTracker : public Playable {
    private:
        std::string mName;
        uint8_t mType;
        std::Array<Channel, 32> mChannels;

    public:

        void Loop() {  }
        bool AtEnd() { return false; }
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        S3MTracker();
        ~S3MTracker();
    };
}


#endif