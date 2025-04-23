#include <audio/Audio.hpp>
#include <algorithm>
#include <iostream>

namespace mb::Audio {
    void Mixer::Update(void *userdata, SDL_AudioStream *stream, int len, int total){
        
        Mixer* mixer = static_cast<Mixer*>(userdata);

        for(auto playable : mixer->mPlaying){
            playable->Mix(mixer->mFrameData, mixer->mWorkBuffer, len);

            if(playable->AtEnd() && !playable->ShouldLoop()){
                std::erase(mixer->mPlaying, playable);
            } else if(playable->AtEnd() && playable->ShouldLoop()){
                playable->Loop();
            }
        }

        // push to audio device
        if(SDL_PutAudioStreamData(stream, mixer->mWorkBuffer, len)){
            mb::Log::Error("Error Pushing Audio to Device: {}", SDL_GetError());
        }
    }
    
    void Mixer::Play(std::string name){
        if(mLoaded.contains(name)){
            mLoaded[name]->Reset();
            mPlaying.push_back(mLoaded[name]);
        }
    }
    
    void Mixer::Pause(std::string name){
        std::vector<std::shared_ptr<Playable>>::iterator toPause = mPlaying.end();
        for(auto it = mPlaying.begin(); it != mPlaying.end(); it++){
            std::cout << "Checking " << (*it)->mName << std::endl;
            if((*it)->mName == name){
                toPause = it;
                break;
            }
        }
        if(toPause != mPlaying.end()){
            std::cout << "Erasing " << (*toPause)->mName << std::endl;
            mPlaying.erase(toPause);
        }
    }
    
    Mixer::Mixer(){
        mb::Log::InfoFrom("MouseyBoxAudio", "Creating Mixer");
        mTargetSpec = {
            .format = SDL_AUDIO_S16,
            .channels = 2,
            .freq = 44100
        };
        
        mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &mTargetSpec, Update, this);
        if(mStream == nullptr){
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Failed");
        } else {
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Complete");
        }

        int sampleCount;
        SDL_AudioSpec deviceSpec;
        SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &deviceSpec, &sampleCount);

        // Allocate enough space for 2 channels of 4 byte per sample samples that will fill our audio buffer
        // This buffer is reused by each formats mix func to store data during mixing as it is guaranteed to have enough space to store the audio data
        mFrameData = new uint8_t[sampleCount * (sizeof(float) * deviceSpec.channels)];
        mWorkBuffer = new uint8_t[sampleCount *(sizeof(int16_t) * deviceSpec.channels)];

        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(mStream));
        SDL_FlushAudioStream(mStream);
    }
    
    Mixer::~Mixer(){
        if(mFrameData != nullptr) delete[] mFrameData;
        if(mWorkBuffer != nullptr) delete[] mWorkBuffer;
        SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    }
}