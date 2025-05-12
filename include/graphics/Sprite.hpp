#ifndef __MB_SPRITE_H__
#define __MB_SPRITE_H__
#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <system/json.hpp>
#include <graphics/Renderable.hpp>


namespace mb::Graphics {

class Renderer;
class SpriteInstance;
class SpriteAnimationInstance;
class SpriteAnimation;

extern bool GlobalSpritePause;

class Sprite {
    int mWidth { 0 }, mHeight { 0 };
    int mTextureWidth { 0 }, mTextureHeight { 0 };
    SDL_Texture* mTexture { nullptr };
    std::map<std::string, std::shared_ptr<SpriteAnimation>> mAnimations;


public:
    SDL_Texture* GetTexture() { return mTexture; }

    int GetTextureWidth() { return mTextureWidth; }
    int GetTextureHeight() { return mTextureHeight; }

    int GetWidth() { return mWidth; }
    int GetHeight() { return mHeight; }
    std::shared_ptr<SpriteAnimation> GetAnimation(std::string name) { if(mAnimations.count(name) != 0) { return mAnimations.at(name); } else { return nullptr; }}

    Sprite(SDL_Renderer*, std::string);
    Sprite(SDL_Renderer*, nlohmann::json&);
    Sprite(SDL_Renderer*, nlohmann::json&, uint8_t*, std::size_t);
    Sprite();
    ~Sprite();
};

class SpriteAnimation {
    friend Sprite;
    friend SpriteAnimationInstance;

    bool mLoop { true };
    float mSpeed { 0.0f };
    uint32_t mFrameCount { 1 };
    std::vector<SDL_FRect> mFrames;


public:
    inline int GetFrameCount(){
        return mFrameCount;
    }

    SpriteAnimation(nlohmann::json);
    SpriteAnimation();
    ~SpriteAnimation(){}
};

class SpriteAnimationInstance {
    friend Sprite;
    std::shared_ptr<SpriteAnimation> mAnimation { nullptr };
    float mFrame { 0.0f };
    bool mIsPaused { false };
    bool mFinished { false };

public:
    inline void Step() {
        if(GlobalSpritePause || mIsPaused) return;
        if(!mAnimation->mLoop){
            if(mFrame + mAnimation->mSpeed < mAnimation->mFrameCount){
                mFrame += mAnimation->mSpeed;
            } else if(!mFinished) {
                mFinished = true;
            }
        } else {
            mFrame = mFrame + mAnimation->mSpeed;
            if(mFrame >= mAnimation->mFrameCount) mFrame = 0.0f;
        }
    }

    void Pause(){
        mIsPaused = true;
    }

    void Unpause(){
        mIsPaused = false;
    }

    inline void SetFrame(int frame){
        mFrame = frame;
    }

    inline void Reset(){
        mFrame = 0;
    }

    inline bool IsFinished() {
        return mFinished;
    }

    SDL_FRect* GetCurrentFrame();
    SpriteAnimationInstance(std::shared_ptr<SpriteAnimation>);
    ~SpriteAnimationInstance(){}
};

class SpriteInstance : public Renderable {
    SDL_FRect mSpriteSrc;

    std::weak_ptr<Sprite> mSprite;
    std::shared_ptr<SpriteAnimationInstance> mCurrentAnimation { nullptr };

public:
    int mOffsetX { 0 }, mOffsetY { 0 };
    float mAngle { 0.0f };
    SDL_FPoint mAnchor { 0.5f, 0.5f };
    SDL_FlipMode mFlip { SDL_FLIP_NONE };
    void Draw(SDL_Renderer*, Camera*) override;

    float GetAngle() { return mAngle; }
    void SetAngle(float a) { mAngle = a; }

    void SetAnchor(float x, float y) { mAnchor.x = x, mAnchor.y = y; }

    void SetAnimation(std::string);
    std::shared_ptr<SpriteAnimationInstance> GetAnimation() { return mCurrentAnimation; }

    SpriteInstance(std::shared_ptr<Sprite>);

    SpriteInstance();
    ~SpriteInstance();
};

}

#endif
