#include <SDL2/SDL_ttf.h>
#include <system/Log.hpp>
#include <graphics/Text.hpp>
#include <graphics/Camera.hpp>

namespace mb::Graphics {

void Text::SetText(SDL_Renderer* renderer, TTF_Font* font, std::string str, int wrap, int align){
    if(str != mText){
        SDL_Surface* textSurface = NULL;
        
        TTF_SetFontWrappedAlign(font, align);
        
        if(wrap > 0){
            textSurface = TTF_RenderText_Blended_Wrapped(font, str.c_str(), mColorFG, wrap);
        } else {
            textSurface = TTF_RenderText_Blended(font, str.c_str(), mColorFG);
        }

        mDrawRect.w = textSurface->w;
        mDrawRect.h = textSurface->h;

        if(textSurface != nullptr){
            mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        }
        
        SDL_FreeSurface(textSurface);
        mText = str;
    }
}

void Text::Draw(SDL_Renderer* r, Camera* cam) {
    if(mTexture != nullptr){
        SDL_Rect rect = mDrawRect;
        if(!mStatic){
            rect.x -= cam->mRect.x;
            rect.y -= cam->mRect.y;
        }
        SDL_SetTextureColorMod(mTexture, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b);
        SDL_RenderCopy(r,  mTexture, nullptr, &rect);
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