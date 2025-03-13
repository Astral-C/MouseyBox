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

        int8_t mFineTune { 0 };

        uint32_t mNote { 0 };
        uint32_t mPeriod { 0 };

        uint32_t mPortaSpeed { 0 };
        uint32_t mPortaPeriod { 0 };

        double mSampleOffset { 0.0 };

        // Check which of these can be made u16s - I wanna smush this!

        uint32_t mDelayTimer { 0 };
        uint32_t mDelayedNote { 0 };

        uint32_t mArpeggioNote1 { 0 };
        uint32_t mArpeggioNote2 { 0 };

        uint32_t mVibratoPos { 0 };
        uint32_t mVibratoSpeed { 0 };
        uint32_t mVibratoDepth { 0 };

        int32_t mTremoloBase { -1 };
        uint32_t mTremoloPos { 0 };
        uint32_t mTremoloSpeed { 0 };
        uint32_t mTremoloDepth { 0 };

        uint16_t mRetriggerTicks { 0 };
        uint16_t mRetriggerCounter { 0 };

        uint8_t mVolSlideDir { 0 };
        uint8_t mVolSlideSpeed { 0 };

        uint8_t mNoteCutTicks { 0 };

    };

}

    class MODTracker : public Playable {
    private:
        std::array<MOD::Channel, 4> mChannels;
        std::array<MOD::Sample, 31> mSamples;
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

        void Loop() { mCurrentPattern = 0; mCurrentRow = 0; mCurrentTicks = 0; }
        bool AtEnd() { return mCurrentPattern == mSongLength && mCurrentRow == 64 && mCurrentTicks == mSpeed; }
        
        void Mix(uint8_t*, int);
        void Load(std::filesystem::path);
        void Load(uint8_t*, size_t);

        MODTracker();
        ~MODTracker();
    };
}


#endif