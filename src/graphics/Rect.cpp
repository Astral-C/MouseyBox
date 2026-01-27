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
    SDL_GetRenderScale(r, &scalex, &scaley);
    SDL_SetRenderScale(r, (float)mThickness, (float)mThickness);

    SDL_FRect rect = mDrawRect;
    if(!mStatic){
        rect.x -= cam->mRect.x;
        rect.y -= cam->mRect.y;
    }

    if(mDrawFilled){
        if(mDropShadow){
            SDL_FRect shadow = { rect.x + mShadowOffsetX, rect.y + mShadowOffsetY, rect.w, rect.h };
            SDL_SetRenderDrawColor(r, 0x00, 0x00, 0x00, mShadowOpacity);
            SDL_RenderFillRect(r, &shadow);
        }
        SDL_SetRenderDrawColor(r, mColorMod.r, mColorMod.g, mColorMod.b, mColorMod.a);
        SDL_RenderFillRect(r, &rect);
    } else {
        // not a fan of this.
        rect.x /= mThickness;
        rect.y /= mThickness;
        rect.w /= mThickness;
        rect.h /= mThickness;
#ifndef __GAMECUBE__
        SDL_RenderRect(r, &rect);
#else
        // Top Bottom
        SDL_RenderLine(r, rect.x, rect.y, rect.x + rect.w, rect.y);
        SDL_RenderLine(r, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
        // Left Right
        SDL_RenderLine(r, rect.x, rect.y, rect.x, rect.y + rect.h);
        SDL_RenderLine(r, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
#endif
    }

    SDL_SetRenderDrawColor(r, tr, tg, tb, ta);
    SDL_SetRenderScale(r, scalex, scaley);
}

Rect::~Rect(){}

}
