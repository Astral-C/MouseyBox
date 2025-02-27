#include <audio/Audio.hpp>
#include <algorithm>
#include <iostream>

namespace mb::Audio {
    void Mixer::Update(void *userdata, SDL_AudioStream *stream, int len, int total){
        uint8_t samples[len] = { 0 };
        
        Mixer* mixer = static_cast<Mixer*>(userdata);

        for(auto playable : mixer->mPlaying){
            playable->Mix(samples, sizeof(samples));

            if(playable->AtEnd() && !playable->ShouldLoop()){
                std::erase(mixer->mPlaying, playable);
            } else if(playable->AtEnd() && playable->ShouldLoop()){
                playable->Loop();
            }
        }

        // push to audio device
        SDL_PutAudioStreamData(stream, samples, sizeof(samples));
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
        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(mStream));
        SDL_FlushAudioStream(mStream);
    }
    
    Mixer::~Mixer(){
        SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    }
}