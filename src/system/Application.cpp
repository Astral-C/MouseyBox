#include <system/Application.hpp>
#include <system/Log.hpp>
#include <iostream>

#ifdef __SWITCH__
#include <switch.h>
#include <unistd.h>
#endif

#ifdef __GAMECUBE__
#include <ogcsys.h>
#include <gccore.h>
#endif

namespace mb {

    namespace {
        bool mSDLReady { false };
    }

    bool Application::SDLReady(){
        return mSDLReady;
    }

    Application::Application(){}
    Application::Application(std::string name){ mApplicationName = name; }

    bool Application::Initialize(){
#ifdef __GAMECUBE__
        SYS_STDIO_Report(true);
#endif

#ifdef __SWITCH__
        romfsInit();
        chdir("romfs:/");
        consoleDebugInit(debugDevice_SVC);
#endif
        mb::Log::Info("MouseyBox", "Creating Application");

        if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0){
            mb::Log::Warn("MouseyBox", "SDL Init failed!");
            SDL_Quit();
            return false;
        }


        mb::Log::Info("MouseyBox", "SDL Ready");


        mSDLReady = true;

        mWindow = std::make_unique<Graphics::Window>(mApplicationName);
        mRenderer = std::make_unique<Graphics::Renderer>();
        mAudio = std::make_unique<Audio::Mixer>();

        mRenderer->Initialize(mWindow.get());
        mb::Log::Info("MouseyBox", "Initialized Base Application");
        return true;
    }
    
    void Application::Update(float dt){
        SDL_Event e;
        SDL_PollEvent(&e);

        if(e.type == SDL_QUIT){
            mQuit = true;
        }

    }

    int Application::SecondsToTicks(int seconds){
        return mTargetFps * seconds;
    }

    void Application::Run(){
#ifndef __SWITCH__
        while(!mQuit)
#else
        while(appletMainLoop() && !mQuit)
#endif
    {

#ifdef __GAMECUBE__
            PAD_ScanPads();
#endif

            Update(mDelta);
            mRenderer->Update();

#ifdef __GAMECUBE__
#else
            mPrevTime = mCurTime;
            mCurTime = SDL_GetTicks64();

            uint64_t curFrameTicks = mCurTime - mPrevTime;

            mDelta = ((float)curFrameTicks / 10000.0f);
            mDelta = 0.001f > mDelta ? 0.001f : mDelta;


            if(curFrameTicks < (1000 / mTargetFps)){
                SDL_Delay((1000 / mTargetFps) - curFrameTicks);
            }
#endif
        }
    }

    Application::~Application(){
        mb::Log::Warn("MouseyBox", "Cleaning up application");
        mRenderer->Cleanup();
        mWindow->Cleanup();
        SDL_Quit();
#ifdef __SWITCH__
        romfsExit();
#endif
    }

}
