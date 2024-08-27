#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerMod.hpp>

namespace mb::Audio {

    void MODTracker::Load(uint8_t* data, size_t size){
        bStream::CMemoryStream stream(data, size, bStream::Endianess::Big, bStream::OpenMode::In);

        mBPM = 125;
        mSpeed = 5;

        stream.seek(0x20);

        for(int i = 0; i < 32; i++){
            MOD::Sample sample;
            stream.readBytesTo((uint8_t*)sample.mName, 22);
            sample.mSampleLength = stream.readUInt16();
            sample.mFineTune = stream.readInt8() & 0b00001111;
            sample.mVolume = stream.readInt8();
            sample.mRepeatOffset = stream.readUInt16();
            sample.mRepeatLength = stream.readUInt16();
            mSamples[i] = sample;
        }

        mSongLength = stream.readUInt8();

        stream.readUInt8(); // old tracker force

        for(int i = 0; i < 128; i++){
            mPositions[i] = stream.readUInt8();
        }

        if(stream.readString(4) == "8chn"){
            mIs8Channel = true;
        }

        uint8_t patternCount = *std::max_element(mPositions.begin(), mPositions.end()) + 1;

        mPatterns.resize(patternCount-1);
        for(int i = 0; i < patternCount; i++){
            stream.readBytesTo((uint8_t*)&mPatterns[i], sizeof(MOD::Pattern));
        }

        for(int i = 0; i < 31; i++){
            if(mSamples[i].mSampleLength <= 0) continue;

            mSamples[i].mData.reserve(mSamples[i].mSampleLength);
            for(int s = 0; s < mSamples[i].mSampleLength; s++){
                mSamples[i].mData.push_back(stream.readInt16());
            }
        }

    }

    void MODTracker::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::Endianess::Big, bStream::OpenMode::In);

        mBPM = 125;
        mSpeed = 5;

        mUpdatesPerTick = 44100 * 2.5 / mBPM;

        mName.resize(20);
        stream.readBytesTo((uint8_t*)mName.data(), 20);

        for(int i = 0; i < 31; i++){
            MOD::Sample sample;
            stream.readBytesTo((uint8_t*)sample.mName, 22);
            mb::Log::Debug("Loaded Sample name is {}", sample.mName);
            sample.mSampleLength = stream.readUInt16();
            sample.mFineTune = stream.readInt8() & 0b00001111;
            sample.mVolume = stream.readInt8();
            sample.mRepeatOffset = stream.readUInt16();
            sample.mRepeatLength = stream.readUInt16();
            mSamples[i] = sample;
        }

        mSongLength = stream.readUInt8();

        stream.readUInt8(); // old tracker force

        for(int i = 0; i < 128; i++){
            mPositions[i] = stream.readUInt8();
        }

        if(stream.readString(4) == "8chn"){
            mIs8Channel = true;
        }

        uint8_t patternCount = *std::max_element(mPositions.begin(), mPositions.end()) + 1;

        for(int i = 0; i < patternCount; i++){
            MOD::Pattern pattern;
            stream.readBytesTo((uint8_t*)&pattern, sizeof(MOD::Pattern));
            mPatterns.push_back(pattern);
        }

        for(int i = 0; i < 31; i++){
            if(mSamples[i].mSampleLength <= 0) continue;

            mSamples[i].mData.resize(mSamples[i].mSampleLength);
            stream.readBytesTo((uint8_t*)mSamples[i].mData.data(), mSamples[i].mSampleLength*2);
            //for(int s = 0; s < mSamples[i].mSampleLength; s++){
                //mSamples[i].mData.push_back(stream.readInt16());
            //}
        }
    }

    void MODTracker::Tick(){
        if(mTickTimer >= mUpdatesPerTick){
            if(mCurrentTicks >= mSpeed){

                int channelIdx = 0;
                for(auto ch : mChannels){
                    uint32_t note = mPatterns[mPositions[mCurrentPattern]].Rows[mCurrentRow][channelIdx];

                    int32_t period = (note & 0x0FFF0000) >> 16;

                    ch.mPrevInstrument = ch.mInstrument;
                    ch.mInstrument = (note & 0xF0000000) >> 24 | (note & 0x0000F000) >> 12;
                    ch.mEffect = (note & 0x00000F00) >> 8;
                    ch.mEffectArgs = (note & 0x000000FF);

                    if(ch.mInstrument >= 0 && ch.mInstrument < 32){
                        ch.mPan = 0x80;
                        ch.mVolume = mSamples[ch.mInstrument].mVolume;
                        if(ch.mInstrument != ch.mPrevInstrument){
                            ch.mSampleOffset = 0;
                        }
                    }

                    if(period > 0){
                        if (ch.mEffect != 0x03 && ch.mEffect != 0x05){
                            ch.mNote = period;
                            ch.mPeriod = period; 
                        }

                        ch.mPortaPeriod = period;
                        ch.mSampleOffset = 0;
                    }

                    channelIdx++;
                }

                mCurrentRow++;
                if(mCurrentRow == 64){
                    mCurrentRow = 0;
                    mCurrentPattern++;
                }

                mCurrentTicks = 0;
            }

            // per tick effects
            mCurrentTicks++;
            mTickTimer = 0;
        }

        mTickTimer++;

    }

    void MODTracker::Mix(uint8_t* data, int len){
        int samplesPerTick, dataPtr = 0;

        int16_t* mixData = (int16_t*)data;

        mb::Log::Debug("More data! from mod");

        while(dataPtr < (len/2)){
            int16_t sampleLeft = 0, sampleRight = 0;
            Tick();

            for(auto channel : mChannels){
                if(channel.mVolume == 0 || channel.mPeriod == 0) continue;

                double freq = (((8363.0 * 428.0) / channel.mPeriod) / mSampleRate);

                MOD::Sample instrument = mSamples[channel.mInstrument];

                int16_t sampleSlice = instrument.mData[static_cast<uint32_t>(channel.mSampleOffset)];
                sampleLeft += (sampleSlice * channel.mVolume * (0xFF - channel.mPan)) / 0xFF;
                sampleRight += (sampleSlice * channel.mVolume * channel.mPan) / 0xFF;

                channel.mSampleOffset += freq;
                if(channel.mSampleOffset > instrument.mSampleLength){
                    if(instrument.mRepeatLength > 0){
                        channel.mSampleOffset = instrument.mRepeatOffset + std::fmod(channel.mSampleOffset, instrument.mRepeatLength);
                    } else {
                        channel.mPeriod = 0;
                        channel.mSampleOffset = 0;
                    }
                }
            }

            //mixData[dataPtr] += sin(dataPtr)*125;
            //mixData[dataPtr+1] += sin(dataPtr)*125;

            mixData[dataPtr] += sampleRight;
            mixData[dataPtr+1] += sampleLeft;
            dataPtr += 2;
        }

    }

    MODTracker::MODTracker(){}

    MODTracker::~MODTracker(){}
} 