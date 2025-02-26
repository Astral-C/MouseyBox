#ifndef __MB_RENDERABLE_H__
#define __MB_RENDERABLE_H__

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <graphics/RenderableType.hpp>

namespace mb::Graphics {

class Camera;

class Renderable {
protected:
    RenderableType mType { RenderableType::None };
    bool mVisible { true };
    int mPriority { 0 };
    float mScale { 1.0f };
    SDL_FRect mDrawRect {};
    SDL_Color mOverlayColor { 255, 255, 255, 255 };
    SDL_Texture* mTexture { nullptr };
    bool mStatic { false };

public:
    virtual void Draw(SDL_Renderer*, Camera*) = 0;
    
    bool IsType(RenderableType t) { return t == mType; }

    int GetPrio() { return mPriority; }
    int* GetSetPrio() { return &mPriority; }
    
    void SetScale(float scale){ mScale = scale; }
    float GetScale(){ return mScale; }
    void SetPriority(int priority){ mPriority = priority; }

    void ToggleVisible() { mVisible = !mVisible; }
    bool IsVisible() { return mVisible; }

    bool GetStatic() { return mStatic; }
    void SetStatic(bool value) { mStatic = value; }

    SDL_FRect* GetRect() { return &mDrawRect; }
    SDL_Color* GetColor() { return &mOverlayColor; }
};

}

#endif