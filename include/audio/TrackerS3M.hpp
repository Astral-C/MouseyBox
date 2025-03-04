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
    class S3MChannel {
        bool mEnabled { false };
        uint8_t mChannelType { 0 };
    };

    class S3MTracker : public Playable {
    private:
        std::string mName;
        uint8_t mType;
        std::array<S3MChannel, 32> mChannels;

        std::vector<uint8_t> mOrders;

        uint8_t mOrderNum { 0 };
        uint8_t mInstrumentCount { 0 };
        uint8_t mPatternCount { 0 };
        uint8_t mFlags { 0 };
        uint8_t mCWT { 0 };
        uint8_t mFormatVersion { 0 };
        uint8_t mGlobalVolume { 0 };
        uint8_t mSpeed { 0 };
        uint8_t mTempo { 0 };
        uint8_t mMasterVolume { 0 };
        uint8_t mSpecial { 0 };

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
