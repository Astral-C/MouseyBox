#ifdef ENABLE_OPENMPT
#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/OpenMPT.hpp>

#include <exception>
#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>
#include <vector>

#include <libopenmpt/libopenmpt.hpp>

namespace mb::Audio {

    void OpenMPT::Load(uint8_t* data, std::size_t size){
        mMod = new openmpt::module(data, size);
    }

    void OpenMPT::Reset(){
    }

    void OpenMPT::Load(std::filesystem::path path){
        std::ifstream stream(path.string(), std::ios::binary);
        mMod = new openmpt::module(stream);
    }

    void OpenMPT::Mix(uint8_t* data, int len){
        int16_t* sampleBuffer = reinterpret_cast<int16_t*>(data);
        mAtEnd = mMod->read_interleaved_stereo(44100, (len / 2) / 2, sampleBuffer);
    }

    void OpenMPT::Loop(){
        // automatic
        mMod->set_position_order_row(0,0);
    }
    
    bool OpenMPT::AtEnd() { 
        return mAtEnd == 0;// mod->get_duration_seconds() == mod->get_position_seconds();
    }
    
    OpenMPT::OpenMPT(){
        mMod = nullptr;
    }

    OpenMPT::~OpenMPT(){
        if(mMod != nullptr){
            delete mMod;
        }
    }
} 
#endif
