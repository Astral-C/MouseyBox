#include <system/json.hpp>
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

Sprite::Sprite(SDL_Renderer* r, nlohmann::json& config){
    int comp;

#ifdef __GAMECUBE__
    stbi_set_flip_vertically_on_load(1);
#endif

    unsigned char* imgData = stbi_load(config["imgPath"].get<std::string>().c_str(), &mWidth, &mHeight, &comp, 4);
    
#ifdef __GAMECUBE__
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mWidth, mHeight, 32, mWidth*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mWidth, mHeight, 32, mWidth*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
    mTexture = SDL_CreateTextureFromSurface(r, surface);

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

    SDL_FreeSurface(surface);
}

Sprite::Sprite(SDL_Renderer* r, nlohmann::json& config, uint8_t* data, size_t size){
    int comp;
    unsigned char* imgData = stbi_load_from_memory(data, size, &mWidth, &mHeight, &comp, 4);
#ifdef __GAMECUBE__
    stbi__vertical_flip(imgData, mWidth, mHeight, 4);
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mWidth, mHeight, 32, mWidth*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mWidth, mHeight, 32, mWidth*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
    mTexture = SDL_CreateTextureFromSurface(r, surface);

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

    SDL_FreeSurface(surface);
}

Sprite::~Sprite(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }
}

SpriteInstance::SpriteInstance(std::shared_ptr<Sprite> sprite){
    mType = RenderableType::Sprite;
    mSprite = sprite;
    mSpriteSrc = {0, 0, sprite->GetWidth(), sprite->GetHeight()};
    mDrawRect = {0, 0, sprite->GetWidth(), sprite->GetHeight()};
    mOverlayColor = {255,255,255,255};
}

SpriteInstance::SpriteInstance(){
    mType = RenderableType::Sprite;
}
SpriteInstance::~SpriteInstance(){}

void SpriteInstance::Draw(SDL_Renderer* r, Camera* cam) {
    std::shared_ptr<Sprite> sprite = mSprite.lock();
    if(sprite != nullptr){
        SDL_Rect* curSrcRect = &mSpriteSrc;
        if(mCurrentAnimation != nullptr){
            curSrcRect = mCurrentAnimation->GetCurrentFrame();
            mCurrentAnimation->Step();
        }

        SDL_Rect draw = mDrawRect;

        if(!mStatic){
            draw.x = mDrawRect.x - cam->mRect.x + mOffsetX;
            draw.y = mDrawRect.y - cam->mRect.y + mOffsetY; 
        }
        
        draw.w *= mScale;
        draw.h *= mScale;

        SDL_SetTextureAlphaMod(sprite->GetTexture(), mOverlayColor.a);
        SDL_SetTextureColorMod(sprite->GetTexture(), mOverlayColor.r, mOverlayColor.g, mOverlayColor.b);
        SDL_RenderCopyEx(r, sprite->GetTexture(), curSrcRect, &draw, mAngle, NULL, mFlip);
        //SDL_RenderDrawRectF(r, &draw);
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

SDL_Rect* SpriteAnimationInstance::GetCurrentFrame(){
     return &mAnimation->mFrames.at(static_cast<uint32_t>(floor(mFrame)) % mAnimation->mFrameCount);
}


}
}
