#ifndef __GAME_H__
#define __GAME_H__
#include <graphics/Sprite.hpp>
#include <system/Application.hpp>

namespace MouseyBoxExample {

class Game : public mb::Application {
private:
public:
    Game();
    ~Game();
    bool Initialize();
    void Update(float dt);

    mb::Graphics::Renderer* GetRenderer(){ return mRenderer.get(); }
    mb::Audio::Mixer* GetAudio(){ return mAudio.get(); }

    void Quit() { mQuit = true; }
};

Game* GetGame();

}

#endif