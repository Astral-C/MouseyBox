#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerMod.hpp>

namespace mb::Audio {

    void MODTracker::Load(uint8_t* data, size_t size){
        //tracker_open_mod_mem(&mModTracker, data, size);
        //tracker_mod_set_sample_rate(&mModTracker, 44100);
    }

    void MODTracker::Load(std::filesystem::path path){
        //tracker_open_mod(&mModTracker, (char*)path.string().c_str());
        //tracker_mod_set_sample_rate(&mModTracker, 44100);
    }

    void MODTracker::Mix(uint8_t* data, int len){
        //tracker_mod_update(&mModTracker, (int16_t*)data, len / 2);
    }

    MODTracker::MODTracker(){}

    MODTracker::~MODTracker(){
        //tracker_close_mod(&mModTracker);
    }
} 