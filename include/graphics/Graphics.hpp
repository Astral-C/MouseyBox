#ifndef __MB_GRAPHICS_H__
#define __MB_GRAPHICS_H__
#include "graphics/RenderableType.hpp"
#include <SDL3/SDL_render.h>
#include <system/json.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_rect.h>
#include <graphics/Pipeline.hpp>
#include <graphics/Text.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/Rect.hpp>
#include <graphics/Camera.hpp>
#include <graphics/Circle.hpp>
#include <string>

namespace mb::Graphics {

class Renderer;

class Window {
private:
    friend Renderer;

    std::string mWindowTitle { "MouseyBox" };
    SDL_Window* mWindow { nullptr };
    SDL_Color mClearColor {0, 0, 0, 0};

public:
    SDL_Window* GetInternalWindow() { return mWindow; }

    void Cleanup();
    SDL_FRect GetSize();

    void SetName(std::string);
    Window(std::string, uint32_t, uint32_t);
    ~Window();
};

class Renderer {
private:
    SDL_GPUDevice* mDevice { nullptr };
    SDL_GPUTexture* mScreenTexture { nullptr };
    SDL_GPURenderState* mPostProcess { nullptr };
    SDL_GPURenderPass* mFrameDefferedPass { nullptr };

    int mWidth { 0 }, mHeight { 0 };
    bool mInFrame { false };
    
    GenericRenderer<CircleInstance> mCircleRenderer;
    ResourceRenderer<ImageResource, SpriteInstance> mSpriteRenderer;

public:
    Camera mCamera;

    void SetSize(int, int);
    SDL_FRect GetSize();

    void Initialize(Window*);

    void BeginFrame();
    void EndFrame();
    void Cleanup();

    template<typename T>
    requires std::derived_from<T, RenderInstance>
    void Draw(T& instance){
        if(!mInFrame) return;

        switch (instance.type) {
            case Circle:
                mCircleRenderer.push(instance);
            default:
                break;
        }
    }

    Renderer();
    ~Renderer();

};


}
#endif
