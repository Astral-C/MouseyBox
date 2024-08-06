#ifndef __MB_CAMERA_H__
#define __MB_CAMERA_H__

#include <system/Entity.hpp>
#include <SDL2/SDL.h>

namespace mb::Graphics {

class Camera {
    SDL_Rect mBounds;
    SDL_Rect mViewportRect {};
    Entity* mFocus { nullptr };

public:
    float mOffsetX { 0.0f }, mOffsetY { 0.0f };
    SDL_Rect mRect {0, 0, 0, 0};

    void Init(SDL_Renderer*);
    void GetFocusedPosition(float* x, float* y);
    void SetFocused(Entity* e){ mFocus = e; }
    void SetBounds(SDL_Rect r) { mBounds = r; }
    void Update(SDL_Renderer*);

    Camera();
    ~Camera();

};

}

#endif