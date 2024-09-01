#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerMod.hpp>

namespace mb::Audio {

    void S3MTracker::Load(uint8_t* data, size_t size){
        //tracker_open_mod_mem(&mModTracker, data, size);
        //tracker_mod_set_sample_rate(&mModTracker, 44100);
    }

    void S3MTracker::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::Endianess::Big, bStream::OpenMode::In);

        mName = stream.readString(28);
        stream.readUInt8(); // 1A
        stream.readUInt8(); // Type

        stream.readUInt16() // 0x0000

        mOrderNum = stream.readUInt8();
        mInstrumentCount = stream.readUInt8();
        mPatternCount = stream.readUInt8();
        mFlags = stream.readUInt8();
        mCWT = stream.readUInt8();
        mFormatVersion = stream.readUInt8();
        stream.readUInt32(); // SCRM
    
        mGlobalVolume = stream.readUInt8();
        mSpeed = stream.readUInt8();
        mTempo = stream.readUInt8();
        mMasterVolume = stream.readUInt8();
    
        stream.readUInt32();
        stream.readUInt32();

        stream.readUInt16();
        mSpecial = stream.readUInt16();

        // read channel settings
        for(int i = 0; i < 32; i++){

        }

    }

    void S3MTracker::Mix(uint8_t* data, int len){
        //tracker_mod_update(&mModTracker, (int16_t*)data, len / 2);
    }

    S3MTracker::S3MTracker(){}

    S3MTracker::~S3MTracker(){
        //tracker_close_mod(&mModTracker);
    }
} 