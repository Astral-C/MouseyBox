#ifndef __MB_CAMERA_H__
#define __MB_CAMERA_H__

#include <system/Entity.hpp>
#include <SDL3/SDL.h>

namespace mb::Graphics {

class Camera {
    SDL_FRect mBounds {};
    SDL_Rect mViewportRect {};
#ifndef DISABLE_ENTITY
    Entity* mFocus { nullptr };
#endif

public:
    float mOffsetX { 0.0f }, mOffsetY { 0.0f };
    SDL_Rect mRect {0, 0, 0, 0};

    void Init(SDL_Renderer*);
#ifndef DISABLE_ENTITY
    void GetFocusedPosition(float* x, float* y);
    void SetFocused(Entity* e){ mFocus = e; }
#endif
    void SetBounds(SDL_FRect r) { mBounds = r; }
    void Update(SDL_Renderer*);

    Camera();
    ~Camera();

};

}

#endif