#include <system/json.hpp>
#include <system/Log.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/Graphics.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <graphics/stb_image.h>
#include <filesystem>
#include <fstream>

namespace mb {
namespace Graphics {

bool GlobalSpritePause = false;

Sprite::Sprite(){}

Sprite::Sprite(SDL_Renderer* r, std::string path){
    int comp;

#ifdef __GAMECUBE__
    stbi_set_flip_vertically_on_load(1);
#endif

    unsigned char* imgData = stbi_load(path.c_str(), &mWidth, &mHeight, &comp, 4);
    
#ifdef __GAMECUBE__
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF), imgData, mWidth*4);
#else
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000), imgData, mWidth*4);
#endif
    mTexture = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_NEAREST);

    mTextureWidth = mWidth;
    mTextureHeight = mHeight;

    stbi_image_free(imgData);

    SDL_DestroySurface(surface);
}

Sprite::Sprite(SDL_Renderer* r, nlohmann::json& config){
    int comp;

#ifdef __GAMECUBE__
    stbi_set_flip_vertically_on_load(1);
#endif

    unsigned char* imgData = stbi_load(config["imgPath"].get<std::string>().c_str(), &mWidth, &mHeight, &comp, 4);
    
#ifdef __GAMECUBE__
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF), imgData, mWidth*4);
#else
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000), imgData, mWidth*4);
#endif
    mTexture = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_NEAREST);

    mTextureWidth = mWidth;
    mTextureHeight = mHeight;

    if(config.contains("width")){
        mWidth = config["width"];
    }

    if(config.contains("height")){
        mHeight = config["height"];
    }

    stbi_image_free(imgData);

    // Load animations

    for(auto animation : config["animations"]){
	std::shared_ptr<SpriteAnimation> anim = std::make_shared<SpriteAnimation>(animation);
        mAnimations.insert({animation["name"].get<std::string>(), anim});
    }

    SDL_DestroySurface(surface);
}

Sprite::Sprite(SDL_Renderer* r, nlohmann::json& config, uint8_t* data, std::size_t size){
    int comp;
    unsigned char* imgData = stbi_load_from_memory(data, size, &mWidth, &mHeight, &comp, 4);
#ifdef __GAMECUBE__
    stbi__vertical_flip(imgData, mWidth, mHeight, 4);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF), imgData, mWidth*4);
#else
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mWidth, mHeight, SDL_GetPixelFormatForMasks(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000), imgData, mWidth*4);
#endif
    mTexture = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_NEAREST);

    mTextureWidth = mWidth;
    mTextureHeight = mHeight;

    if(config.contains("width")){
        mWidth = config["width"];
    }

    if(config.contains("height")){
        mHeight = config["height"];
    }

    stbi_image_free(imgData);

    // Load animations

    for(auto animation : config["animations"]){
	std::shared_ptr<SpriteAnimation> anim = std::make_shared<SpriteAnimation>(animation);
        mAnimations.insert({animation["name"].get<std::string>(), anim});
    }

    SDL_DestroySurface(surface);
}

Sprite::~Sprite(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }
}

SpriteInstance::SpriteInstance(std::shared_ptr<Sprite> sprite){
    mType = RenderableType::Sprite;
    mSprite = sprite;
    mSpriteSrc = {0, 0, static_cast<float>(sprite->GetWidth()), static_cast<float>(sprite->GetHeight())};
    mDrawRect = {0, 0, static_cast<float>(sprite->GetWidth()), static_cast<float>(sprite->GetHeight())};
    mOverlayColor = {255,255,255,255};
}

SpriteInstance::SpriteInstance(){
    mType = RenderableType::Sprite;
}
SpriteInstance::~SpriteInstance(){}

void SpriteInstance::Draw(SDL_Renderer* r, Camera* cam) {
    std::shared_ptr<Sprite> sprite = mSprite.lock();
    if(sprite != nullptr){
        SDL_FRect* curSrcRect = &mSpriteSrc;
        if(mCurrentAnimation != nullptr){
            curSrcRect = mCurrentAnimation->GetCurrentFrame();
            mCurrentAnimation->Step();
        }

        SDL_FRect draw = mDrawRect;

        if(!mStatic){
            draw.x = mDrawRect.x - cam->mRect.x + mOffsetX;
            draw.y = mDrawRect.y - cam->mRect.y + mOffsetY; 
        }
        
        draw.w *= mScale;
        draw.h *= mScale;

        // draw rect _MUST_ always be whole number! This prevents sheet bleeding.
        draw.x = round(draw.x);
        draw.y = round(draw.y);
        draw.w = round(draw.w);
        draw.h = round(draw.h);

        SDL_SetTextureAlphaMod(sprite->GetTexture(), mOverlayColor.a);
        SDL_SetTextureColorMod(sprite->GetTexture(), mOverlayColor.r, mOverlayColor.g, mOverlayColor.b);
        SDL_RenderTextureRotated(r, sprite->GetTexture(), curSrcRect, &draw, mAngle, &mAnchor, mFlip);
        //SDL_RenderRectF(r, &draw);
    }
}

void SpriteInstance::SetAnimation(std::string name){
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<SpriteAnimationInstance> anim;
    if(sprite = mSprite.lock()){
        if(sprite->GetAnimation(name) != nullptr){
            anim = std::make_shared<SpriteAnimationInstance>(sprite->GetAnimation(name));
            if(anim != nullptr){
                mCurrentAnimation = anim;
            }
        }
    }
}

SpriteAnimation::SpriteAnimation(nlohmann::json config){
    mFrameCount = config["frames"].size();
#ifdef __GAMECUBE__
    mSpeed = config["speed"].get<float>() * 2.0f;
#else
    mSpeed = config["speed"];
#endif    
    mLoop = config["loop"];
    for(auto frame : config["frames"]){
        mFrames.push_back({frame[0],frame[1],frame[2],frame[3]});
    }
}

SpriteAnimationInstance::SpriteAnimationInstance(std::shared_ptr<SpriteAnimation> a){
    mAnimation = a;
    mFrame = 0;
}

SDL_FRect* SpriteAnimationInstance::GetCurrentFrame(){
    return &mAnimation->mFrames.at(static_cast<uint32_t>(floor(mFrame)) % mAnimation->mFrameCount);
}


}
}
