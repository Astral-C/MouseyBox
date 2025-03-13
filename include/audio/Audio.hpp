#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL3/SDL_audio.h>
#include <system/Log.hpp>
#include <audio/Playable.hpp>
#include <filesystem>

#include <map>
#include <vector>
#include <memory>

namespace mb::Audio {
    class Mixer {
        float volume { 0.5f };
        SDL_AudioStream* mStream { nullptr };
        SDL_AudioSpec mDeviceSpec {};
        SDL_AudioSpec mTargetSpec {
            .format = SDL_AUDIO_S16,
            .channels = 2,
            .freq = 44100
        };

        std::map<std::string, std::shared_ptr<Playable>> mLoaded {};
        std::vector<std::shared_ptr<Playable>> mPlaying {};

        static void Update(void *userdata, SDL_AudioStream *stream, int len, int total);
        
    public:
        void UpdateAudio();

        template<typename T>
        std::shared_ptr<T> Load(std::string name, std::filesystem::path path){
            static_assert(std::is_base_of_v<Playable, T>);
            std::shared_ptr<T> playable = std::make_shared<T>();
            
            playable->Load(path);
            playable->SetTargetSpec(&mTargetSpec);
            
            mLoaded.insert({name, playable});
            return playable;
        }

        template<typename T>
        std::shared_ptr<T> Load(std::string name, uint8_t* data, std::size_t size){
            static_assert(std::is_base_of_v<Playable, T>);
            std::shared_ptr<T> playable = std::make_shared<T>();
            
            playable->mName = name;
            playable->Load(data, size);
            playable->SetTargetSpec(&mTargetSpec);
            
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