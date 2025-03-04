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
        bStream::CFileStream stream(path.string(), bStream::Endianess::Big, bStream::OpenMode::In);

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
        mMasterVolume = stream.readUInt8();
    
        stream.seek(0x3E);
        mSpecial = stream.readUInt16();

        // read channel settings
        for(std::size_t i = 0; i < 32; i++){
            uint8_t settings = strema.readUInt8();
            mChannels[i].mEnabled = (settings & 0x80 >= 0;
            mChannels[i].mType = (settings & 0x7F);
        }

        stream.seek(0x60);
        mOrders.reserve(mOrderNum);
        for(std::size_t i = 0; i < mOrderNum; i ++){
            mOrders.push_back(stream.readUInt8());
        }

        for (std::size_t i = 0; i < mPatternCount; i++) {
            uint32_t patternOffs = static_cast<uint32_t>(stream.readUInt16())*16;
        
            // seek to and read pattern

            std::size_t pos = stream.tell();
            stream.seek(patternOffs);
            mPatterns.push_back()
        }

        for (std::size_t i = 0; i < mInstrumentCount; i++) {
            mInstrumentPointers.push_back(static_cast<uint32_t>(stream.readUInt16())*16);

            // seek to and read instrument
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
