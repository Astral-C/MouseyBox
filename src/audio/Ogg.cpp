#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/Ogg.hpp>
#ifdef STB_VORBIS_HEADER_ONLY
#undef STB_VORBIS_HEADER_ONLY
#endif
#include <audio/formats/stb_vorbis.h>

namespace mb::Audio {

    void Ogg::Load(uint8_t* data, size_t size){
        int err = 0;
        mVorbisHandle = stb_vorbis_open_memory(data, size, &err, NULL);

        if(err != 0){
            return; //This need better error checking
        }
    }

    void Ogg::Reset(){
        stb_vorbis_seek_start(mVorbisHandle);
    }

    void Ogg::Load(std::filesystem::path path){
        int err = 0;
        mVorbisHandle = stb_vorbis_open_filename(path.string().c_str(), &err, NULL);

        if(err != 0){
            mb::Log::Error("Failed to load OGG: {}", err);
            return; //This need better error checking
        }
    }

    void Ogg::Mix(uint8_t* frameBuffer, uint8_t* data, int len){
        int16_t* FrameData = reinterpret_cast<int16_t*>(frameBuffer);
        memset(FrameData, 0, (len / sizeof(int16_t)));
        
        stb_vorbis_get_samples_short_interleaved(mVorbisHandle, 2, FrameData, len/sizeof(int16_t));

        int16_t* copyBuffer = (int16_t*)data;

        for (size_t i = 0; i < len/sizeof(int16_t); i++){
            copyBuffer[i] += FrameData[i];
        }
    }

    void Ogg::Loop(){
        stb_vorbis_seek_start(mVorbisHandle);
    }
    
    bool Ogg::AtEnd() { 
        return stb_vorbis_get_sample_offset(mVorbisHandle) >= stb_vorbis_stream_length_in_samples(mVorbisHandle);
    }
    
    Ogg::Ogg(){}

    Ogg::~Ogg(){
        stb_vorbis_close(mVorbisHandle);
    }
} 