#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <system/Log.hpp>
#include <graphics/Graphics.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/DynText.hpp>
#include <graphics/Particle.hpp>
#include <graphics/stb_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace mb {
namespace Graphics {

Window::Window(std::string name, uint32_t w, uint32_t h){
    mb::Log::InfoFrom("MouseyBox", "Creating Window");
    mWindowTitle = name;

    SDL_SetHint(SDL_HINT_APP_ID, "mouseybox");

    mWindow = SDL_CreateWindow(mWindowTitle.data(), w, h, SDL_WINDOW_RESIZABLE);

    if(mWindow == nullptr){
        Log::Error("Error creating SDL Window {}", SDL_GetError());
        SDL_Quit();
    }
}

Window::~Window(){}

void Window::SetName(std::string name){
    SDL_SetWindowTitle(mWindow, name.data());
}

void Window::Cleanup(){
    SDL_DestroyWindow(mWindow);
}

SDL_FRect Window::GetSize(){
    int x { 0 }, y { 0 }, w { 0 }, h { 0 };
    SDL_GetWindowPosition(mWindow, &x, &y);
    SDL_GetWindowSize(mWindow, &w, &h);
    return { (float)x, (float)y, (float)w, (float)h };
}

Renderer::Renderer(){}
Renderer::~Renderer(){}

void Renderer::Initialize(Window* win){
    mb::Log::InfoFrom("MouseyBox", "Initing Renderer");
    if(!win && !win->mWindow){
        Log::Error("No Window on Renderer Init!");
        return;
    }

    if(TTF_Init() < 0){
        Log::Error("Unable to init SDL_ttf! Text rendering won't work");
    }

    mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);

    if(mDevice == nullptr){
        Log::InfoFrom("MouseyBox", "Error Creating GPU Device: {}", SDL_GetError());
        SDL_Quit();
    }
    
    if(!SDL_ClaimWindowForGPUDevice(mDevice, win->mWindow)){
        Log::InfoFrom("MouseyBox", "Error Claiming Window for GPU Device: {}", SDL_GetError());
        SDL_Quit();
    }

    mb::Log::InfoFrom("MouseyBox", "Initing Camera");
    
    mCamera.Init();
    
    SDL_GetWindowSize(win->GetInternalWindow(), &mWidth, &mHeight);
    
    SDL_GPUTextureCreateInfo screen_texture_create_info = (SDL_GPUTextureCreateInfo){
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = 1280, .height = 720, .layer_count_or_depth = 1, .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_2
    };

    mScreenTexture = SDL_CreateGPUTexture(mDevice, &screen_texture_create_info);

    mb::Log::InfoFrom("MouseyBox", "Renderer Initizlized");
}

SDL_FRect Renderer::GetSize(){
    SDL_FRect rendererRect {0, 0, (float)mWidth, (float)mHeight};
    return rendererRect;
}

void Renderer::SetSize(int w, int h){
    mWidth = w;
    mHeight = h;

    SDL_ReleaseGPUTexture(mDevice, mScreenTexture);

    SDL_GPUTextureCreateInfo screen_texture_create_info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = 1280, .height = 720, .layer_count_or_depth = 1, .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1
    };

    mScreenTexture = SDL_CreateGPUTexture(mDevice, &screen_texture_create_info);

    
    mCamera.mRect.w = mWidth;
    mCamera.mRect.h = mHeight;
}

void Renderer::BeginFrame(){
    SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(mDevice);

    SDL_GPUColorTargetInfo screen_pass_target {
        .texture = mScreenTexture,
        .clear_color = {0.0f , 0.0f, 0.0f, 0.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    mFrameDefferedPass = SDL_BeginGPURenderPass(cmd_buffer, &screen_pass_target, 1, nullptr);

}

void Renderer::EndFrame(){
    SDL_EndGPURenderPass(mFrameDefferedPass);

    
}

void Renderer::Cleanup(){
    if(mPostProcess != nullptr){
        SDL_DestroyGPURenderState(mPostProcess);
    }
    if(mScreenTexture != nullptr){
        SDL_ReleaseGPUTexture(mDevice, mScreenTexture);
    }
    if(mDevice != nullptr){
        SDL_DestroyGPUDevice(mDevice);
    }
    TTF_Quit();
}

}
}
