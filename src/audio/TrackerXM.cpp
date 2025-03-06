#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerXM.hpp>

namespace mb::Audio {

    void TrackerXM::Load(uint8_t* data, std::size_t size){
        if(xm_create_context_safe(&mXMHandle, (const char*)data, size, 44100) != 0){
          mb::Log::Error("Failed to Load XM Module from data");
          return;
        }
        mLastPattern = xm_get_module_length(mXMHandle)-1;
        mLastPatternRows = xm_get_number_of_rows(mXMHandle, mLastPattern);
    }

    void TrackerXM::Reset(){
        //xm_seek(mXMHandle, 0, 0, 0);
    }

    void TrackerXM::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::OpenMode::In);
        
        std::size_t streamSize = stream.getSize();
        
        uint8_t* data = new uint8_t[streamSize]{0};
        stream.readBytesTo(data, streamSize);

        if(xm_create_context_safe(&mXMHandle, (const char*)data, streamSize, 44100) != 0){
          mb::Log::Error("Failed to Load XM Module from data");
          return;
        }
        mLastPattern = xm_get_module_length(mXMHandle)-1;
        mLastPatternRows = xm_get_number_of_rows(mXMHandle, mLastPattern); 
    
        delete[] data;
    }

    void TrackerXM::Mix(uint8_t* data, int len){
        uint16_t* sampleBuffer = reinterpret_cast<uint16_t*>(data);

        float samples[len]{0.0}; // msvc may hate this...
        xm_generate_samples(mXMHandle, samples, len/sizeof(float));
        
        for(int sample = 0; sample < (len/2); sample+=2){
            sampleBuffer[sample] += (int16_t)((samples[sample] * INT16_MAX) * mVolume);
            sampleBuffer[sample+1] += (int16_t)((samples[sample+1] * INT16_MAX) * mVolume);
        }
    }

    void TrackerXM::Loop(){
        // This is automatic
        //xm_seek(mXMHandle, 0, 0, 0);
    }
    
    bool TrackerXM::AtEnd() { 
       uint8_t curPattern, curRow;
       xm_get_position(mXMHandle, nullptr, &curPattern, &curRow, nullptr);
       return mLastPattern == curPattern && mLastPatternRows == curRow;
    }
    
    TrackerXM::TrackerXM(){}

    TrackerXM::~TrackerXM(){
        xm_free_context(mXMHandle);
    }
} 

