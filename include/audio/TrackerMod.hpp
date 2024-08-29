#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>
#include <cstdint>
#include <audio/Playable.hpp>
#include <bStream/bstream.h>
#include <filesystem>
#include <vector>
#include <array>
#include <set>

namespace mb::Audio {

namespace MOD {
    
    struct Pattern {
        uint32_t Rows[64][4];
    };

    struct Sample {
        char mName[22];
        uint16_t mSampleLength; //in u16s
        int8_t mFineTune;       // First nybble should be blank, next is finetune
        int8_t mVolume;         //max is 64, 0x00 - 0x40
        uint16_t mRepeatOffset; //in u16s
        uint16_t mRepeatLength; //in u16s
        
        std::vector<int8_t> mData;

        int16_t operator[](int idx){ return mData.at(idx); }

    };

    struct Channel {
        int8_t mVolume { 0x40 };
        uint8_t mPrevInstrument;
        uint8_t mInstrument;
        uint8_t mEffect;
        uint8_t mEffectArgs;
        uint8_t mPan;

        uint32_t mNote;
        uint32_t mPeriod;

        uint32_t mPortaSpeed;
        uint32_t mPortaPeriod;

        double mSampleOffset;

        uint32_t mDelayTimer;
        uint32_t mDelayedNote;

        uint32_t mArpeggioNote1;
        uint32_t mArpeggioNote2;

        uint32_t mVibratoDepth;
        uint32_t mVibratoPos;


    };

}

    class MODTracker : public Playable {
    private:
        std::array<MOD::Channel, 4> mChannels;
        std::array<MOD::Sample, 32> mSamples;
        std::array<uint8_t, 128> mPositions;

        std::vector<MOD::Pattern> mPatterns;

        uint32_t mTickTimer;
        uint32_t mCurrentTicks;
        uint32_t mUpdatesPerTick;
        uint32_t mSampleRate { 44100 };

        uint16_t mBPM;
        uint16_t mSpeed;
        uint16_t mCurrentRow;
        uint16_t mCurrentPattern;

        uint8_t mLoopRow;
        uint8_t mLoopCount;

        uint8_t mSongLength;

        bool mIs8Channel { false };

        void Tick();

    public:

        void Loop() {  }
        bool AtEnd() { return mCurrentPattern == mSongLength; }
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        MODTracker();
        ~MODTracker();
    };
}


#endif