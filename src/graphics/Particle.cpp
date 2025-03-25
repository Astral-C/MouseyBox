#include <system/json.hpp>
#include <system/Log.hpp>
#include <graphics/Particle.hpp>
#include <graphics/Graphics.hpp>
#include <graphics/stb_image.h>
#include <filesystem>
#include <fstream>

namespace mb {
namespace Graphics {

ParticleSystem::ParticleSystem(){}

ParticleSystem::ParticleSystem(SDL_Renderer* r, nlohmann::json& config){
    mType = RenderableType::ParticleSystem;
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

    if(config.contains("particle_max")){
        mParticleMax = config["particle_max"];
    } else {
        mParticleMax = 10;
    }

    mParticles = new Particle[mParticleMax];

    stbi_image_free(imgData);

    SDL_DestroySurface(surface);
}

ParticleSystem::ParticleSystem(SDL_Renderer* r, nlohmann::json& config, uint8_t* data, std::size_t size){
    mType = RenderableType::ParticleSystem;

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

    if(config.contains("particle_max")){
        mParticleMax = config["particle_max"];
    } else {
        mParticleMax = 10;
    }

    mParticles = new Particle[mParticleMax];

    stbi_image_free(imgData);

    SDL_DestroySurface(surface);
}

ParticleSystem::~ParticleSystem(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }

    if(mParticles != nullptr){
        delete[] mParticles;
    }
}

void ParticleSystem::Draw(SDL_Renderer* r, Camera* cam) {
    if(mParticles == nullptr) return;
    
    SDL_FRect draw { 0, 0, 0, 0 };    
    
    draw.w = mWidth * mScale;
    draw.h = mHeight * mScale;

    for(int i = 0; i < mParticleMax; i++){
        draw.x = mParticles[i].mPosition.x;
        draw.y = mParticles[i].mPosition.y;
        SDL_SetTextureAlphaMod(mTexture, mOverlayColor.a);
        SDL_SetTextureColorMod(mTexture, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b);
        SDL_RenderTextureRotated(r, mTexture, nullptr, &draw, 0.0f, NULL, SDL_FlipMode::SDL_FLIP_NONE);
    
        mParticles[i].mPosition = mParticles[i].mPosition + mParticles[i].mVelocity;
        mParticles[i].mVelocity = mParticles[i].mVelocity + mParticles[i].mAcceleration;
    }
}

}
}
