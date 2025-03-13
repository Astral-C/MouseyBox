#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerS3M.hpp>

namespace mb::Audio {

    void S3MTracker::Load(uint8_t* data, size_t size){
        //tracker_open_mod_mem(&mModTracker, data, size);
        //tracker_mod_set_sample_rate(&mModTracker, 44100);
    }

    void S3MTracker::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::Endianess::Little, bStream::OpenMode::In);

        mName = stream.readString(28);
        stream.readUInt8(); // 1A
        stream.readUInt8(); // Type

        stream.readUInt16(); // 0x0000

        mOrderNum = stream.readUInt16();
        mInstrumentCount = stream.readUInt16();
        mPatternCount = stream.readUInt16();
        mFlags = stream.readUInt16();
        mCWT = stream.readUInt16();
        mFormatVersion = stream.readUInt16();
        stream.readUInt32(); // SCRM
    
        mGlobalVolume = stream.readUInt8();
        mSpeed = stream.readUInt8();
        mTempo = stream.readUInt8();
        mMixingVolume = stream.readUInt8();
        stream.readUInt8(); // click removal for gravis ultrasound
        mPanning = stream.readUInt8(); 

        stream.readUInt32();
        stream.readUInt32();

        stream.readUInt16();

        /*
        // read channel settings
        for(int i = 0; i < 32; i++){
            uint8_t channelSettings = stream.readUInt8();
            if(channelSettings & 0b10000000){
                mChannels[i].mEnabled = true;
            }
            mChannels[i].mChannelType = static_cast<S3MChannelType>(channelSettings & 0b01111111);
        }
            */

        

    }

    void S3MTracker::Mix(uint8_t* data, int len){
        //tracker_mod_update(&mModTracker, (int16_t*)data, len / 2);
    }

    S3MTracker::S3MTracker(){}

    S3MTracker::~S3MTracker(){
        //tracker_close_mod(&mModTracker);
    }
} 
