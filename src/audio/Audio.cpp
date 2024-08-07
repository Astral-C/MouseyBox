#include <audio/Audio.hpp>
#include <algorithm>
#include <iostream>

namespace mb::Audio {

    void Mixer::UpdateAudio(void* userdata, uint8_t* stream, int len){
        memset(stream, 0, len);
        Mixer* mixer = static_cast<Mixer*>(userdata);
        for(auto playable : mixer->mPlaying){
            playable->Mix(stream, len);
            
            /*
            if(playable->AtEnd() && !playable->ShouldLoop()){
                std::remove(mixer->mPlaying.begin(), mixer->mPlaying.end(), playable);
            } else if(playable->AtEnd() && playable->ShouldLoop()){
                playable->Loop();
            }*/
        }
    }

    void Mixer::Play(std::string name){
        if(mLoaded.contains(name)){
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
        mb::Log::InfoFrom("MouseyBox", "Creating Mixer");
        mTargetSpec = {
            .freq = 44100,
            .format = AUDIO_S16,
            .channels = 2,
            .samples = 4096,
            .callback = UpdateAudio,
            .userdata = this
        };

        mDeviceID = SDL_OpenAudioDevice(NULL, 0, &mTargetSpec, &mDeviceSpec, 0);
        if(mDeviceID < 0){
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Failed");
        } else {
            mb::Log::DebugFrom("MouseyBoxAudio", "Mixer Setup Complete");
        }
        SDL_PauseAudioDevice(mDeviceID, 0);
    }

    Mixer::~Mixer(){
        SDL_CloseAudioDevice(mDeviceID);
    }
}