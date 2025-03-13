#include <graphics/Camera.hpp>
#include <system/Log.hpp>
#include <format>

namespace mb::Graphics{

Camera::Camera(){}
Camera::~Camera(){}

void Camera::Init(SDL_Renderer* r){
    SDL_GetRenderViewport(r, &mViewportRect);
    mb::Log::Debug(std::format("Viewport {} {} {} {}", mViewportRect.x, mViewportRect.y, mViewportRect.w, mViewportRect.h));
    mRect.w = mViewportRect.w;
    mRect.h = mViewportRect.h;
}

#ifndef DISABLE_ENTITY
void Camera::GetFocusedPosition(float* x, float* y){
    if(mFocus == nullptr){
        *x = 0.0f;
        *y = 0.0f;
    } else {
        *x = mFocus->mX;
        *y = mFocus->mY;
    }
}
#endif

void Camera::Update(SDL_Renderer* r){
    SDL_GetRenderViewport(r, &mViewportRect);

#ifndef DISABLE_ENTITY
    if(mFocus != nullptr){
        mRect.x = (mFocus->mX + (mFocus->GetRect().w / 2)) - (mViewportRect.w / 2)  + mOffsetX;
        mRect.y = (mFocus->mY + (mFocus->GetRect().h / 2)) - (mViewportRect.h / 2)  + mOffsetY;
    }
#endif

    if(mBounds.w != 0 && mBounds.h != 0){
        if(mRect.x < mBounds.x){
            mRect.x = mBounds.x;
        } else if(mRect.x + mViewportRect.w  > mBounds.x + mBounds.w){
            mRect.x = ((mBounds.x + mBounds.w) - mViewportRect.w);
        }

        if(mRect.y < mBounds.y){
            mRect.y = mBounds.y;
        } else if(mRect.y + mViewportRect.h  > mBounds.y + mBounds.h){
            mRect.y = ((mBounds.y + mBounds.h) - mViewportRect.h);
        }

    }
}

}