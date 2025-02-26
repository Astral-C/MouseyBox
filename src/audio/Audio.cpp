#include <audio/Audio.hpp>
#include <algorithm>
#include <iostream>

namespace mb::Audio {
    void Mixer::UpdateAudio(){
        uint8_t samples[4096] = {0};
        for(auto playable : mPlaying){
            playable->Mix(samples, sizeof(samples));
            
            if(playable->AtEnd() && !playable->ShouldLoop()){
                std::erase(mPlaying, playable);
            } else if(playable->AtEnd() && playable->ShouldLoop()){
                playable->Loop();
            }
        }
        // push to audio device
        SDL_PutAudioStreamData(mStream, samples, sizeof(samples));
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

        mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &mTargetSpec, NULL, NULL);
        if(mStream == nullptr){
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Failed");
        } else {
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Complete");
        }
        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(mStream));
    }

    Mixer::~Mixer(){
        SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    }
}