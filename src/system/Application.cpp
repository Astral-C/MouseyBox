#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
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

    bool Application::Initialize(bool commandline, uint32_t w, uint32_t h){
#ifdef __GAMECUBE__
        SYS_STDIO_Report(true);
#endif

#ifdef __SWITCH__
        romfsInit();
        std::filesystem::current_path("romfs:/");
        consoleDebugInit(debugDevice_SVC);
#endif
        mb::Log::InfoFrom("MouseyBox", "Creating Application");

        if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
            mb::Log::WarnFrom("MouseyBox", "SDL Video Subsystem Failed!");
            SDL_Quit();
            return false;
        }

        if(!SDL_InitSubSystem(SDL_INIT_AUDIO)){
            mb::Log::WarnFrom("MouseyBox", "SDL Audio Subsystem Failed!");
            SDL_Quit();
            return false;
        }

        if(!SDL_InitSubSystem(SDL_INIT_EVENTS)){
            mb::Log::WarnFrom("MouseyBox", "SDL Event Subsystem Failed!");
            SDL_Quit();
            return false;
        }

        if(!SDL_InitSubSystem(SDL_INIT_JOYSTICK)){
            mb::Log::WarnFrom("MouseyBox", "SDL Joystick Subsystem Failed!");
            SDL_Quit();
            return false;
        }


        mb::Log::InfoFrom("MouseyBox", "SDL Ready");


        mSDLReady = true;

        if(commandline){
            mb::Log::InfoFrom("MouseyBox", "Done Creating Commandline Application");
            return true;
        }

        mWindow = std::make_unique<Graphics::Window>(mApplicationName, w, h);
        mRenderer = std::make_unique<Graphics::Renderer>();
        mAudio = std::make_unique<Audio::Mixer>();

        mRenderer->Initialize(mWindow.get());

        mb::Log::InfoFrom("MouseyBox", "Initialized Base Application");
        return true;
    }

    void Application::Update(float dt){
        SDL_Event e;
        SDL_PollEvent(&e);

        if(e.type == SDL_EVENT_QUIT){
            mQuit = true;
        }

    }

    void Application::Frame(){
        Update(mDelta);
        SDL_FRect winRect = mWindow->GetSize();
        if(mRenderer != nullptr) mRenderer->Update(winRect.w, winRect.h);

        mPrevTime = mCurTime;
        mCurTime = SDL_GetTicks();

        uint64_t curFrameTicks = mCurTime - mPrevTime;

        mDelta = ((float)curFrameTicks / 10000.0f);
        mDelta = 0.001f > mDelta ? 0.001f : mDelta;
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
            SDL_FRect winRect = mWindow->GetSize();
            if(mRenderer != nullptr) mRenderer->Update(winRect.w, winRect.h);

#ifdef __GAMECUBE__
#else
            mPrevTime = mCurTime;
            mCurTime = SDL_GetTicks();

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
        mb::Log::WarnFrom("MouseyBox", "Cleaning up application");
        mRenderer->Cleanup();
        mWindow->Cleanup();
        SDL_Quit();
#ifdef __SWITCH__
        romfsExit();
#endif
    }

}
