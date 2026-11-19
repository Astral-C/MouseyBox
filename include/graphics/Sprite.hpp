#ifndef __MB_SPRITE_H__
#define __MB_SPRITE_H__
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <system/Log.hpp>
#include <system/Util.hpp>
#include <system/json.hpp>
#include <graphics/Pipeline.hpp>
#include <graphics/RenderableType.hpp>


namespace mb::Graphics {

struct ImageResource : GraphicsResource {
    int32_t mTextureWidth { 0 }, mTextureHeight { 0 };
    SDL_GPUTexture* mTexture { nullptr };

    int GetWidth() { return mTextureWidth; }
    int GetHeight() { return mTextureHeight; }

    ImageResource();
    ~ImageResource();
};


struct SpriteInstance : RenderInstance {
    int32_t mImageResourceID;
};


}

#endif
