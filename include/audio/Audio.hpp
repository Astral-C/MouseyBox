#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL2/SDL_audio.h>
#include <system/Log.hpp>
#include <audio/Playable.hpp>
#include <filesystem>

#include <map>
#include <vector>
#include <memory>

namespace mb::Audio {
    class Mixer {
        float volume { 0.5f };
        SDL_AudioDeviceID mDeviceID {};
        SDL_AudioSpec mDeviceSpec {};
        SDL_AudioSpec mTargetSpec {
            .freq = 44100,
            .format = AUDIO_S16,
            .channels = 2,
            .samples = 4096
        };

        std::map<std::string, std::shared_ptr<Playable>> mLoaded {};
        std::vector<std::shared_ptr<Playable>> mPlaying {};

        static void UpdateAudio(void*,uint8_t*,int);
    
    public:

        template<typename T>
        std::shared_ptr<T> Load(std::filesystem::path path){
            static_assert(std::is_base_of_v<Playable, T>);
            std::shared_ptr<T> playable = std::make_shared<T>();
            
            playable->Load(path);
            
            mLoaded.insert({path.stem().string(), playable});
            return playable;
        }

        template<typename T>
        std::shared_ptr<T> Load(std::string name, uint8_t* data, size_t size){
            static_assert(std::is_base_of_v<Playable, T>);
            std::shared_ptr<T> playable = std::make_shared<T>();
            
            playable->mName = name;
            playable->Load(data, size);
            
            mLoaded.insert({name, playable});
            return playable;
        }

        void Pause(std::string);
        void Play(std::string);

        Mixer();
        ~Mixer();

    };
}

#endif