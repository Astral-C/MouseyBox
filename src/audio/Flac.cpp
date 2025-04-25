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

    void Flac::Mix(uint8_t* frameBuffer, uint8_t* workBuffer, int len){
        if(mFlacHandle == nullptr) return;

        std::size_t requestedSize = len / sizeof(int16_t);

        int16_t* FrameData = reinterpret_cast<int16_t*>(frameBuffer);

        drflac_read_pcm_frames_s16(mFlacHandle, requestedSize / 2, FrameData);
        
        int16_t* destBuffer = reinterpret_cast<int16_t*>(workBuffer);
        for (size_t i = 0; i < requestedSize; i++){
            destBuffer[i] += FrameData[i] * mVolume;
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