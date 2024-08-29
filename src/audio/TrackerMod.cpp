#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerMod.hpp>

namespace mb::Audio {
    static uint16_t finetune_table[16][36] = {
        {
            856,808,762,720,678,640,604,570,538,508,480,453, // C-1 to B-1 Finetune 0
            428,404,381,360,339,320,302,285,269,254,240,226, // C-2 to B-2 Finetune 0
            214,202,190,180,170,160,151,143,135,127,120,113 // C-3 to B-3 Finetune 0
        },
        {
            850,802,757,715,674,637,601,567,535,505,477,450, // C-1 to B-1 Finetune +1
            425,401,379,357,337,318,300,284,268,253,239,225, // C-2 to B-2 Finetune +1
            213,201,189,179,169,159,150,142,134,126,119,113 // C-3 to B-3 Finetune +1
        },
        {
            844,796,752,709,670,632,597,563,532,502,474,447, // C-1 to B-1 Finetune +2
            422,398,376,355,335,316,298,282,266,251,237,224, // C-2 to B-2 Finetune +2
            211,199,188,177,167,158,149,141,133,125,118,112 // C-3 to B-3 Finetune +2
        },
        {
            838,791,746,704,665,628,592,559,528,498,470,444, // C-1 to B-1 Finetune +3
            419,395,373,352,332,314,296,280,264,249,235,222, // C-2 to B-2 Finetune +3
            209,198,187,176,166,157,148,140,132,125,118,111 // C-3 to B-3 Finetune +3
        },
        {
            832,785,741,699,660,623,588,555,524,495,467,441, // C-1 to B-1 Finetune +4
            416,392,370,350,330,312,294,278,262,247,233,220, // C-2 to B-2 Finetune +4
            208,196,185,175,165,156,147,139,131,124,117,110 // C-3 to B-3 Finetune +4
        },
        {
            826,779,736,694,655,619,584,551,520,491,463,437, // C-1 to B-1 Finetune +5
            413,390,368,347,328,309,292,276,260,245,232,219, // C-2 to B-2 Finetune +5
            206,195,184,174,164,155,146,138,130,123,116,109 // C-3 to B-3 Finetune +5
        },
        {
            820,774,730,689,651,614,580,547,516,487,460,434, // C-1 to B-1 Finetune +6
            410,387,365,345,325,307,290,274,258,244,230,217, // C-2 to B-2 Finetune +6
            205,193,183,172,163,154,145,137,129,122,115,109 // C-3 to B-3 Finetune +6
        },
        {
            814,768,725,684,646,610,575,543,513,484,457,431, // C-1 to B-1 Finetune +7
            407,384,363,342,323,305,288,272,256,242,228,216, // C-2 to B-2 Finetune +7
            204,192,181,171,161,152,144,136,128,121,114,108 // C-3 to B-3 Finetune +7
        },
        {
            907,856,808,762,720,678,640,604,570,538,504,480, // C-1 to B-1 Finetune -8
            453,428,404,381,360,339,320,302,285,269,254,240, // C-2 to B-2 Finetune -8
            226,214,202,190,180,170,160,151,143,135,127,120 // C-3 to B-3 Finetune -8
        },
        {
            900,850,802,757,715,675,636,601,567,535,505,477, // C-1 to B-1 Finetune -7
            450,425,401,379,357,337,318,300,284,268,253,238, // C-2 to B-2 Finetune -7
            225,212,200,189,179,169,159,150,142,134,126,119 // C-3 to B-3 Finetune -7
        },
        {
            894,844,796,752,709,670,632,597,563,532,502,474, // C-1 to B-1 Finetune -6
            447,422,398,376,355,335,316,298,282,266,251,237, // C-2 to B-2 Finetune -6
            223,211,199,188,177,167,158,149,141,133,125,118 // C-3 to B-3 Finetune -6
        },
        {
            887,838,791,746,704,665,628,592,559,528,498,470, // C-1 to B-1 Finetune -5
            444,419,395,373,352,332,314,296,280,264,249,235, // C-2 to B-2 Finetune -5
            222,209,198,187,176,166,157,148,140,132,125,118 // C-3 to B-3 Finetune -5
        },
        {
            881,832,785,741,699,660,623,588,555,524,494,467, // C-1 to B-1 Finetune -4
            441,416,392,370,350,330,312,294,278,262,247,233, // C-2 to B-2 Finetune -4
            220,208,196,185,175,165,156,147,139,131,123,117 // C-3 to B-3 Finetune -4
        },
        {
            875,826,779,736,694,655,619,584,551,520,491,463, // C-1 to B-1 Finetune -3
            437,413,390,368,347,338,309,292,276,260,245,232, // C-2 to B-2 Finetune -3
            219,206,195,184,174,164,155,146,138,130,123,116 // C-3 to B-3 Finetune -3
        },
        {
            868,820,774,730,689,651,614,580,547,516,487,460, // C-1 to B-1 Finetune -2
            434,410,387,365,345,325,307,290,274,258,244,230, // C-2 to B-2 Finetune -2
            217,205,193,183,172,163,154,145,137,129,122,115 // C-3 to B-3 Finetune -2
        },
        {
            862,814,768,725,684,646,610,575,543,513,484,457, // C-1 to B-1 Finetune -1
            431,407,384,363,342,323,305,288,272,256,242,228, // C-2 to B-2 Finetune -1
            216,203,192,181,171,161,152,144,136,128,121,114  // C-3 to B-3 Finetune -1
        }
    };

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

        uint8_t patternCount = *std::max_element(mPositions.begin(), mPositions.end());

        mPatterns.resize(patternCount);
        for(int i = 0; i < patternCount; i++){
            for(int r = 0; r < 64; r++){
                for(int c = 0; c < 4; c++){
                    mPatterns[i].Rows[r][c] = stream.readUInt32();
                }
            }
        }

        for(int i = 0; i < 31; i++){
            if(mSamples[i].mSampleLength <= 0) continue;

            mSamples[i].mData.reserve(mSamples[i].mSampleLength);
            for(int s = 0; s < mSamples[i].mSampleLength; s++){
                mSamples[i].mData.push_back(stream.readInt8());
            }
        }

    }

    void MODTracker::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::Endianess::Big, bStream::OpenMode::In);

        mBPM = 125;
        mSpeed = 6;

        mUpdatesPerTick = 44100 * 2.5 / mBPM;

        stream.readString(20);

        for(int i = 0; i < 31; i++){
            MOD::Sample sample;
            stream.readBytesTo((uint8_t*)sample.mName, 22);
            mb::Log::Debug("Loaded Sample name is {}", sample.mName);
            sample.mSampleLength = stream.readUInt16() * 2;
            sample.mFineTune = stream.readInt8() & 0b00001111;
            sample.mVolume = stream.readInt8();
            sample.mRepeatOffset = stream.readUInt16() * 2;
            sample.mRepeatLength = stream.readUInt16() * 2;
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

        uint32_t patternCount = 0;
        for(int i = 0; i < 128; i++){
            if(mPositions[i] > patternCount){
                patternCount = mPositions[i];
            }
        }

        mPatterns.resize(patternCount+1);
        for(int i = 0; i < patternCount+1; i++){
            for(int r = 0; r < 64; r++){
                for(int c = 0; c < 4; c++){
                    mPatterns[i].Rows[r][c] = stream.readUInt32();
                }
            }
        }
        mb::Log::Debug("Reading Sample Data at {}", stream.tell());
        for(int i = 0; i < 31; i++){
            if(mSamples[i].mSampleLength <= 0) continue;

            mSamples[i].mData.reserve(mSamples[i].mSampleLength);
            for(int s = 0; s < mSamples[i].mSampleLength; s++){
                mSamples[i].mData.push_back(stream.readInt8());
            }
        }

    }

    void MODTracker::Tick(){
        if(mTickTimer >= mUpdatesPerTick){
            if(mCurrentTicks >= mSpeed){
                int channelIdx = 0;
                for(int i = 0; i < mChannels.size(); i++){
                    bool trigger = true;
                    uint32_t note = mPatterns[mPositions[mCurrentPattern]].Rows[mCurrentRow][i];

                    uint32_t period = (note & 0x0FFF0000) >> 16;
                    mChannels[i].mPrevInstrument = mChannels[i].mInstrument;
                    uint32_t instrument = (note & 0xF0000000) >> 24 | (note & 0x0000F000) >> 12;
                    mChannels[i].mEffect = (note & 0x00000F00) >> 8;
                    mChannels[i].mEffectArgs = (note & 0x000000FF);

                    if(instrument > 0 && instrument <= 32){
                        mChannels[i].mInstrument = instrument;
                        mChannels[i].mPan = 0x80;
                        mChannels[i].mVolume = mSamples[mChannels[i].mInstrument].mVolume;
                        if(mChannels[i].mInstrument != mChannels[i].mPrevInstrument){
                            mChannels[i].mSampleOffset = 0;
                        }
                    }
                    // Per Tick Effects, first tick events
                    switch (mChannels[i].mEffect){
                        case 0x3:
                            trigger = false;
                            if(period == 0) break;
                            mChannels[i].mPortaPeriod = period;
                            mChannels[i].mPortaSpeed = (mChannels[i].mEffectArgs & 0x0F);
                            mb::Log::Debug("Starting Tone Porta from {} to {} with speed {}", mChannels[i].mPeriod, mChannels[i].mPortaPeriod, mChannels[i].mPortaSpeed);
                            break;
                        default:
                            break;
                    }

                    if(period > 0 && trigger){
                        mb::Log::Debug("Triggering Note w/ period {}", period);
                        mChannels[i].mNote = period;
                        mChannels[i].mPeriod = period;
                        mChannels[i].mSampleOffset = 0;
                    }
                }

                mCurrentRow++;
                if(mCurrentRow == 64){
                    mCurrentRow = 0;
                    mCurrentPattern++;
                }

                mCurrentTicks = 0;
            }

            // Inter tick effects
            for(int i = 0; i < mChannels.size(); i++){
                switch (mChannels[i].mEffect){
                case 0x3:
                    if(mChannels[i].mPeriod < mChannels[i].mPortaPeriod){

                        mChannels[i].mPeriod += mChannels[i].mPortaSpeed;

                        if(mChannels[i].mPeriod >= mChannels[i].mPortaPeriod){
                            mChannels[i].mPeriod = mChannels[i].mPortaPeriod;
                        }
                    } else if(mChannels[i].mPeriod > mChannels[i].mPortaPeriod){

                        mChannels[i].mPeriod -= mChannels[i].mPortaSpeed;

                        if(mChannels[i].mPeriod <= mChannels[i].mPortaPeriod){
                            mChannels[i].mPeriod = mChannels[i].mPortaPeriod;
                        }
                    }
                    mb::Log::Debug("Processing Porta from {} -> {}", mChannels[i].mPeriod, mChannels[i].mPortaPeriod);
                    break;
                case 0xA:
                    if((mChannels[i].mEffectArgs & 0xF0) >> 4 == 0) {
                        mChannels[i].mVolume -= mChannels[i].mEffectArgs & 0x0F;
                    }
                    else if((mChannels[i].mEffectArgs & 0x0F) == 0) {
                        mChannels[i].mVolume += (mChannels[i].mEffectArgs & 0xF0) >> 4;
                    }
                    break;
                
                default:
                    break;
                }
            }

            // per tick effects
            mCurrentTicks++;
            mTickTimer = 0;
        }

        mTickTimer++;

    }

    uint32_t GetTruePeriod(uint32_t period, uint8_t finetune){
        uint32_t truePeriod = 0;
        size_t noteIdx = 0;
        uint32_t closestDist = 0xFFFFFFFF;

        for (size_t i = 0; i < 36; i++){
            if(closestDist == 0) break;
            if(abs((int)finetune_table[finetune][i] - (int)period) < closestDist){
                closestDist = abs((int)finetune_table[finetune][i] - (int)period);
                noteIdx = i;
            }
        }

        return finetune_table[finetune][noteIdx];
        
    }

    void MODTracker::Mix(uint8_t* data, int len){
        int samplesPerTick, dataPtr = 0;

        int16_t* mixData = (int16_t*)data;


        while(dataPtr < (len/2)){
            int16_t sampleLeft = 0, sampleRight = 0;
            Tick();

            for(int c = 0; c < mChannels.size(); c++){
                auto channel = mChannels[c];
                if(channel.mVolume == 0 || channel.mPeriod == 0) continue;

                MOD::Sample instrument = mSamples[channel.mInstrument];
                // 3546895?
                double freq = (((8363.0 * 428.0) / GetTruePeriod(channel.mPeriod, instrument.mFineTune + 8)) / mSampleRate);

                int16_t sampleSlice = instrument.mData[static_cast<uint32_t>(channel.mSampleOffset)];

                sampleLeft += (sampleSlice * channel.mVolume * (0xFF - channel.mPan)) / 0xFF;
                sampleRight += (sampleSlice * channel.mVolume * channel.mPan) / 0xFF;

                mChannels[c].mSampleOffset += freq;
                if(instrument.mRepeatLength >= 2 && mChannels[c].mSampleOffset >= instrument.mRepeatOffset + instrument.mRepeatLength){
                    mChannels[c].mSampleOffset = instrument.mRepeatOffset + std::fmod(mChannels[c].mSampleOffset, instrument.mRepeatLength);
                } else if(instrument.mRepeatLength < 2 && channel.mSampleOffset >= instrument.mSampleLength){
                    mChannels[c].mPeriod = 0;
                    mChannels[c].mSampleOffset = 0;
                }
            }

            mixData[dataPtr] += sampleRight;
            mixData[dataPtr+1] += sampleLeft;
            dataPtr += 2;
        }

    }

    MODTracker::MODTracker(){}

    MODTracker::~MODTracker(){}
} 