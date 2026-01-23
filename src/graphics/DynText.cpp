#include "bStream/bstream.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <system/Log.hpp>
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include <graphics/DynText.hpp>
#include <graphics/Camera.hpp>

namespace mb::Graphics {

//TODO: Set up a way to add custom glyphs or select from specific ranges in the packer, leave it ascii for now

DynFont::DynFont(SDL_Renderer* r, std::string fontPath, uint32_t fontSize, uint32_t charCount){
    stbtt_pack_context ctx;

    uint8_t* fontData = nullptr;
    std::size_t fontFileSize = 0;

    {
        bStream::CFileStream fontStream(fontPath, bStream::Endianess::Little, bStream::OpenMode::In);

        fontFileSize = fontStream.getSize();
        fontData = new uint8_t[fontFileSize];
        fontStream.readBytesTo(fontData, fontFileSize);
    }

    mGlyphMetrics.resize(charCount);

    uint32_t atlasRes = fontSize * fontSize * charCount;
    uint8_t* atlasData = new uint8_t[atlasRes];

    stbtt_PackBegin(&ctx, atlasData, fontSize * charCount, fontSize, 0, 1, nullptr);
    stbtt_PackSetOversampling(&ctx, 1, 1);
    stbtt_PackFontRange(&ctx, fontData, 0, fontSize, 32, charCount, mGlyphMetrics.data());
    stbtt_PackEnd(&ctx);


    mGlyphAtlas = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, fontSize * charCount, fontSize);

    uint32_t* atlasTexture = new uint32_t[atlasRes];
    const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32);

    for(int p = 0; p < atlasRes; p++){
        atlasTexture[p] = SDL_MapRGBA(format, nullptr, 0xFF, 0xFF, 0xFF, atlasData[p]);
    }

    SDL_UpdateTexture(mGlyphAtlas, nullptr, atlasTexture, fontSize*charCount*4);

    // Get some extra font metricds
    if(stbtt_InitFont(&mFontInfo, fontData, 0) == 0){
        return;
    }

    mFontSize = fontSize;
    mFontScale = stbtt_ScaleForPixelHeight(&mFontInfo, fontSize);
    stbtt_GetFontVMetrics(&mFontInfo, &mAscent, &mDescent, &mLineGap);
    mAscent *= mFontScale;
    mDescent *= mFontScale;
    mLineGap *= mFontScale;

    delete[] atlasTexture;
    delete[] atlasData;
    delete[] fontData;
}

DynFont::~DynFont(){
    if(mGlyphAtlas != nullptr){
        SDL_DestroyTexture(mGlyphAtlas);
    }
}

void DynText::SetText(std::string str){
}

void DynText::AddTextChunk(uint32_t effect, std::string text){
    mText.push_back({ .mEffect = static_cast<DynTextEffect>(effect), .mText = text });
}

void DynText::Draw(SDL_Renderer* r, Camera* cam) {
    std::shared_ptr<DynFont> font;

    if(!(font = mFont.lock())){
        return;
    }

    if(font->mGlyphAtlas != nullptr){
        SDL_FRect rect = mDrawRect;
        if(!mStatic){
            rect.x -= cam->mRect.x;
            rect.y -= cam->mRect.y;
        }

        SDL_FRect cursor = rect;

        // some parsing for text commands should be here?
        for(auto& chunk : mText){
            if(chunk.mText.size() == 0) continue;

            if(chunk.mEffect & COLOR && chunk.mEffect & ~RAINBOW && chunk.mEffectArg0 < mPalette.size()){
                SDL_SetTextureColorMod(font->mGlyphAtlas, mPalette[chunk.mEffectArg0].r, mPalette[chunk.mEffectArg0].g, mPalette[chunk.mEffectArg0].b);
            } else {
                SDL_SetTextureColorMod(font->mGlyphAtlas, 0xFF, 0xFF, 0xFF);
            }

            for (int i = 0; i < chunk.mText.size(); i++){
                char c = chunk.mText[i];
                if(i > 0) cursor.x += stbtt_GetCodepointKernAdvance(&font->mFontInfo, chunk.mText[i-1], c);
                stbtt_packedchar charInfo = font->mGlyphMetrics[c - 32];
                SDL_FRect src { static_cast<float>(charInfo.x0), static_cast<float>(charInfo.y0), static_cast<float>(charInfo.x1) - static_cast<float>(charInfo.x0), static_cast<float>(charInfo.y1) - static_cast<float>(charInfo.y0) };
                SDL_FRect dst { cursor.x + charInfo.xoff, cursor.y + charInfo.yoff, static_cast<float>(charInfo.x1) - static_cast<float>(charInfo.x0), static_cast<float>(charInfo.y1) - static_cast<float>(charInfo.y0) };

                if(chunk.mEffect & FLOAT && chunk.mEffect & ~SHAKE){
                    dst.y += sinf(chunk.mTime + (static_cast<float>(i) / 1)) * chunk.mEffectArg0;
                } else if(chunk.mEffect & SHAKE) {
                    dst.x += (rand() % (chunk.mEffectArg0 * 2)) * 0.5f;
                    dst.y += (rand() % (chunk.mEffectArg0 * 2)) * 0.5f;
                }

                if(chunk.mEffect & RAINBOW){
                    SDL_SetTextureColorMod(font->mGlyphAtlas, mPalette[(static_cast<int>(chunk.mTime) + i) % mPalette.size()].r, mPalette[(static_cast<int>(chunk.mTime) + i) % mPalette.size()].g, mPalette[(static_cast<int>(chunk.mTime) + i) % mPalette.size()].b);
                }

                if(dst.x + dst.w > rect.x + rect.w){
                    cursor.x = rect.x;
                    cursor.y += font->mFontSize + font->mLineGap;
                } else {
                    cursor.x += charInfo.xadvance;
                }

                SDL_RenderTexture(r, font->mGlyphAtlas, &src, &dst);
            }
            chunk.mTime += chunk.mEffectArg1;
            cursor.x += font->mGlyphMetrics[' '-32].xadvance; // auto add space after each chunk
        }

    }
}

DynText::DynText(){
    mType = RenderableType::DynamicText;
}

DynText::~DynText(){}

}
