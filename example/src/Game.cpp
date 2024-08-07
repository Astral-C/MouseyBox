#include <Game.hpp>
#include <system/Input.hpp>
#include <system/Archive.hpp>
#include <system/Message.hpp>
#include <system/Script.hpp>
#include <system/Entity.hpp>
#include <system/Math.hpp>
#include <system/Log.hpp>
#include <format>
#include <iostream>
#include <fstream>

#ifdef __GAMECUBE__
#include "HelvetiPixel_ttf.h"
#include <ogcsys.h>
#include <gccore.h>
#endif

namespace MouseyBoxExample {

namespace {
    Game App;
}

Game* GetGame(){
    return &App;
}

Game::Game() : mb::Application("Example") {}

void ExampleEntityUpdate(mb::Entity* self){
    self->mY = ((App.GetRenderer()->GetSize().h / 2) - (self->GetRect().h / 2)) + (15 * sin(self->mHealth++ * 0.05f));
}

bool Game::Initialize() {
#ifdef __GAMECUBE__
    PAD_Init();
#endif
    if(!Application::Initialize()){
        return false;
    }

    SDL_ShowCursor(SDL_DISABLE);

    mb::Input::Initialize();
    mb::Input::OpenJoysticks(1);

    mb::Log::SetSrc("Example");

    mb::Entity::Initialize(500);
    
#ifndef __GAMECUBE__
    // HelvetiPixel.ttf should be in your build folder!
    mRenderer->LoadFont("HelvetiPixel.ttf", 16, "HelvetiPixel16");
    mRenderer->LoadFont("HelvetiPixel.ttf", 32, "HelvetiPixel32");
    mRenderer->LoadFont("HelvetiPixel.ttf", 64, "HelvetiPixel64");
#else
    mRenderer->LoadFontFromMemory("HelvetiPixel16", (uint8_t*)HelvetiPixel_ttf, HelvetiPixel_ttf_size, 16);
    mRenderer->LoadFontFromMemory("HelvetiPixel32", (uint8_t*)HelvetiPixel_ttf, HelvetiPixel_ttf_size, 32);
    mRenderer->LoadFontFromMemory("HelvetiPixel64", (uint8_t*)HelvetiPixel_ttf, HelvetiPixel_ttf_size, 64);
#endif

    mb::Entity* exampleInt = mb::Entity::New();
    exampleInt->mUpdate = ExampleEntityUpdate;

    std::weak_ptr<mb::Graphics::Text> text = mRenderer->CreateText("HelvetiPixel32", "Example Entity!");
    exampleInt->mX = (mRenderer->GetSize().w / 2) - (text.lock()->GetRect()->w / 2);

    exampleInt->SetRenderable(text.lock());

    mb::Log::Debug("Game Ready");
    return true;
}

Game::~Game(){
    mb::Log::Info("Cleaning up Game");
    mb::Entity::Cleanup();
    mb::Input::Cleanup();
}

void MouseyBoxExample::Game::Update(float dt) {
    mb::Input::Update();

    SDL_Event e;
    SDL_PollEvent(&e);

    if(e.type == SDL_QUIT){
        mQuit = true;
    }

    mb::Entity::Update();
}

}