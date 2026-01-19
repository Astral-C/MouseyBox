#include "bStream/bstream.h"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <system/Log.hpp>
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include <graphics/DynText.hpp>
#include <graphics/Camera.hpp>

namespace mb::Graphics {

DynFont::DynFont(SDL_Renderer* r, std::string fontPath, uint32_t fontSize, uint32_t charCount){
    stbtt_pack_context ctx;

    //bStream::CFileStream fontStream(fontPath, );

    //stbtt_InitFont(&mFontInfo, fileData, 0);

    uint32_t atlasRes = fontSize * fontSize * charCount;
    uint8_t* atlasData = new uint8_t[atlasRes];

    stbtt_PackBegin(&ctx, atlasData, fontSize*charCount, fontSize, 0, 1, nullptr);

    mGlyphAtlas = SDL_CreateTexture(r, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STATIC, fontSize * charCount, fontSize);

    stbtt_PackEnd(&ctx);
    delete[] atlasData;
}

DynFont::~DynFont(){
    if(mGlyphAtlas != nullptr){
        SDL_DestroyTexture(mGlyphAtlas);
    }
}

void DynText::SetText(SDL_Renderer* renderer, TTF_Font* font, std::string str, int wrap, TTF_HorizontalAlignment align){
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

void DynText::Draw(SDL_Renderer* r, Camera* cam) {
    if(mTexture != nullptr){
        SDL_FRect rect = mDrawRect;
        if(!mStatic){
            rect.x -= cam->mRect.x;
            rect.y -= cam->mRect.y;
        }
        SDL_SetTextureColorMod(mTexture, mColorMod.r, mColorMod.g, mColorMod.b);

        rect.w *= mScale;
        rect.h *= mScale;

        SDL_RenderTexture(r,  mTexture, nullptr, &rect);
    }
}

DynText::DynText(){
    mType = RenderableType::Text;
}

DynText::~DynText(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }
}

}
