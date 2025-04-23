#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/Flac.hpp>

#define DR_FLAC_IMPLEMENTATION
#include <audio/formats/dr_flac.h>

namespace mb::Audio {

    void Flac::Load(uint8_t* data, size_t size){
    }

    void Flac::Reset(){
        drflac_seek_to_pcm_frame(mFlacHandle, 0);
    }

    void Flac::Load(std::filesystem::path path){
        mFlacHandle = drflac_open_file(path.string().c_str(), NULL);

        if(mFlacHandle == nullptr){
            mb::Log::Error("Error Loading Flac");
        }
    }

    void Flac::Mix(uint8_t* frameBuffer, uint8_t* data, int len){
        if(mFlacHandle == nullptr) return;

        int16_t* FrameData = reinterpret_cast<int16_t*>(frameBuffer);
        memset(FrameData, 0, (len / sizeof(int16_t)));

        drflac_read_pcm_frames_s16(mFlacHandle, (len / sizeof(int16_t)) / 2, FrameData);
        
        int16_t* copyBuffer = (int16_t*)data;
        for (size_t i = 0; i < len/sizeof(int16_t); i++){
            copyBuffer[i] += FrameData[i] * mVolume;
        }
    }

    void Flac::Loop(){
        drflac_seek_to_pcm_frame(mFlacHandle, 0);
    }
    
    bool Flac::AtEnd() { 
        if(mFlacHandle->currentPCMFrame >= mFlacHandle->totalPCMFrameCount){
            return true;
        } else{
            return false;
        }
    }
    
    Flac::Flac(){}

    Flac::~Flac(){
        drflac_close(mFlacHandle);
    }
} 