#include <graphics/Camera.hpp>
#include <graphics/Rect.hpp>

namespace mb::Graphics {

Rect::Rect(){
    mType = RenderableType::Rect;
}

void Rect::Draw(SDL_Renderer* r, Camera* cam) {
    float scalex, scaley;
    uint8_t tr, tg, tb, ta;
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_GetRenderDrawColor(r, &tr, &tg, &tb, &ta);
    SDL_SetRenderDrawColor(r, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b, mOverlayColor.a);
    SDL_RenderGetScale(r, &scalex, &scaley);
    SDL_RenderSetScale(r, (float)mThickness, (float)mThickness);

    SDL_Rect rect = mDrawRect;
    if(!mStatic){
        rect.x -= cam->mRect.x;
        rect.y -= cam->mRect.y;
    }

    if(mDrawFilled){
        SDL_RenderFillRect(r, &rect);
    } else {
        // not a fan of this.
        rect.x /= mThickness;
        rect.y /= mThickness;
        rect.w /= mThickness;
        rect.h /= mThickness;
#ifndef __GAMECUBE__
        SDL_RenderDrawRect(r, &rect);
#else
        // Top Bottom
        SDL_RenderDrawLine(r, rect.x, rect.y, rect.x + rect.w, rect.y);
        SDL_RenderDrawLine(r, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
        // Left Right
        SDL_RenderDrawLine(r, rect.x, rect.y, rect.x, rect.y + rect.h);
        SDL_RenderDrawLine(r, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
#endif
    }

    SDL_SetRenderDrawColor(r, tr, tg, tb, ta);
    SDL_RenderSetScale(r, scalex, scaley);
}

Rect::~Rect(){}

}