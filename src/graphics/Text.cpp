#include <SDL3_ttf/SDL_ttf.h>
#include <system/Log.hpp>
#include <graphics/Text.hpp>
#include <graphics/Camera.hpp>

namespace mb::Graphics {

void Text::SetText(SDL_Renderer* renderer, TTF_Font* font, std::string str, int wrap, TTF_HorizontalAlignment align){
    if(str != mText){
        SDL_Surface* textSurface = NULL;
        
        TTF_SetFontWrapAlignment(font, align);
        
        if(wrap > 0){
            textSurface = TTF_RenderText_Blended_Wrapped(font, str.c_str(), str.size(), mColorFG, wrap);
        } else {
            textSurface = TTF_RenderText_Blended(font, str.c_str(), str.size(), mColorFG);
        }

        mDrawRect.w = textSurface->w;
        mDrawRect.h = textSurface->h;

        if(textSurface != nullptr){
            mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_NEAREST);
        }
        
        SDL_DestroySurface(textSurface);
        mText = str;
    }
}

void Text::Draw(SDL_Renderer* r, Camera* cam) {
    if(mTexture != nullptr){
        SDL_FRect rect = mDrawRect;
        if(!mStatic){
            rect.x -= cam->mRect.x;
            rect.y -= cam->mRect.y;
        }
        SDL_SetTextureColorMod(mTexture, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b);
        SDL_RenderTexture(r,  mTexture, nullptr, &rect);
    }
}

Text::Text(){
    mType = RenderableType::Text;
}

Text::~Text(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }
}

}