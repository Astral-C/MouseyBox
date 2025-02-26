#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/Wav.hpp>

namespace mb::Audio {

    void Wav::Load(uint8_t* data, std::size_t size){
        SDL_IOStream* wav = SDL_IOFromMem(data, size);
        SDL_LoadWAV_IO(wav, true, &mWavSpec, static_cast<Uint8**>(&mData), &mDataLen);

        //TODO: Error Check
    }

    void Wav::Reset(){
        mSampleOffset = 0;
    }

    void Wav::Load(std::filesystem::path path){
        SDL_LoadWAV(path.string().c_str(), &mWavSpec, static_cast<Uint8**>(&mData), &mDataLen);
        
        if(mWavSpec.format == SDL_AUDIO_F32){
            mDataLen /= 4;
        } else if(mWavSpec.format == SDL_AUDIO_S16){
            mDataLen /= 2;
        }
        //TODO: Error Check
    
    }

    void Wav::Mix(uint8_t* data, int len){
        uint16_t* sampleBuffer = reinterpret_cast<uint16_t*>(data);
        if(mSampleOffset < mDataLen){
            for(int sample = 0; sample < len/2; sample+=2){
                
                if(mSampleOffset >= mDataLen) break;

                switch (mWavSpec.format) {
                case SDL_AUDIO_F32:
                    sampleBuffer[sample] += (int16_t)((reinterpret_cast<float*>(mData)[mSampleOffset] * INT16_MAX) * mVolume);
                    sampleBuffer[sample+1] += (int16_t)((reinterpret_cast<float*>(mData)[mSampleOffset+1] * INT16_MAX) * mVolume);
                    break;
                
                case SDL_AUDIO_S16:
                    sampleBuffer[sample] += (int16_t)(reinterpret_cast<int16_t*>(mData)[mSampleOffset] * mVolume);
                    sampleBuffer[sample+1] += (int16_t)(reinterpret_cast<int16_t*>(mData)[mSampleOffset+1] * mVolume);
                    break;

                default:
                    sampleBuffer[sample] = 0;
                    sampleBuffer[sample+1] = 0;
                    break;
                }
                mSampleOffset += (mWavSpec.freq / mTargetSpec->freq) * 2; 
            }
        }
    }

    void Wav::Loop(){
        mSampleOffset = 0;
    }
    
    bool Wav::AtEnd() { 
        return mSampleOffset >= mDataLen; 
    }
    
    Wav::Wav(){}

    Wav::~Wav(){
        SDL_free(mData);
    }
} 