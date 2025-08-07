#include <system/json.hpp>
#include <system/Log.hpp>
#include <graphics/Particle.hpp>
#include <graphics/Graphics.hpp>
#include <graphics/stb_image.h>
#include <filesystem>
#include <fstream>
#include <random>

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

    if(config.contains("spawn_area_width")){
        mDrawRect.w = config["spawn_area_width"];
    } else {
        mDrawRect.w = 10;
    }

    if(config.contains("spawn_area_height")){
        mDrawRect.w = config["spawn_area_height"];
    } else {
        mDrawRect.w = 10;
    }    

    int lifemin, lifemax;

    if(config.contains("min_lifetime")){
        lifemin = config["min_lifetime"];
    } else {
        lifemin = 100;
    }

    if(config.contains("max_lifetime")){
        lifemax = config["max_lifetime"];
    } else {
        lifemax = 200;
    }

    float velxmin, velxmax;

    if(config.contains("min_vel_x")){
        velxmin = config["min_vel_x"];
    } else {
        velxmin = 0.0f;
    }

    if(config.contains("max_vel_x")){
        velxmax = config["max_vel_x"];
    } else {
        velxmax = 1.0f;
    }

    float velymin, velymax;

    if(config.contains("min_vel_y")){
        velymin = config["min_vel_y"];
    } else {
        velymin = 0.0f;
    }

    if(config.contains("max_vel_y")){
        velymax = config["max_vel_y"];
    } else {
        velymax = 1.0f;
    }

    float rmin, rmax;

    if(config.contains("min_vel_rot")){
        rmin = config["min_vel_rot"];
    } else {
        rmin = 0.0f;
    }

    if(config.contains("max_vel_rot")){
        rmax = config["max_vel_rot"];
    } else {
        rmax = 1.0f;
    }

    mParticles = new Particle[mParticleMax];
    mGen = std::mt19937(mRand());
    mDistX = std::uniform_int_distribution<int>(0, mDrawRect.w);
    mDistY = std::uniform_int_distribution<int>(0, mDrawRect.h);
    mLifeDist = std::uniform_int_distribution<int>(lifemin, lifemax);
    mVelXDist = std::uniform_real_distribution<float>(velxmin, velxmax);
    mVelYDist = std::uniform_real_distribution<float>(velymin, velymax);
    mVelRDist = std::uniform_real_distribution<float>(rmin, rmax);

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

    if(config.contains("spawn_area_width")){
        mDrawRect.w = config["spawn_area_width"];
    } else {
        mDrawRect.w = 10;
    }

    if(config.contains("spawn_area_height")){
        mDrawRect.w = config["spawn_area_height"];
    } else {
        mDrawRect.w = 10;
    }    

    int lifemin, lifemax;

    if(config.contains("min_lifetime")){
        lifemin = config["min_lifetime"];
    } else {
        lifemin = 100;
    }

    if(config.contains("max_lifetime")){
        lifemax = config["max_lifetime"];
    } else {
        lifemax = 200;
    }

    float velxmin, velxmax;

    if(config.contains("min_vel_x")){
        velxmin = config["min_vel_x"];
    } else {
        velxmin = 0.0f;
    }

    if(config.contains("max_vel_x")){
        velxmax = config["max_vel_x"];
    } else {
        velxmax = 1.0f;
    }

    float velymin, velymax;

    if(config.contains("min_vel_y")){
        velymin = config["min_vel_y"];
    } else {
        velymin = 0.0f;
    }

    if(config.contains("max_vel_y")){
        velymax = config["max_vel_y"];
    } else {
        velymax = 1.0f;
    }

    float rmin, rmax;

    if(config.contains("min_vel_rot")){
        rmin = config["min_vel_rot"];
    } else {
        rmin = 0.0f;
    }

    if(config.contains("max_vel_rot")){
        rmax = config["max_vel_rot"];
    } else {
        rmax = 1.0f;
    }

    mParticles = new Particle[mParticleMax];
    mGen = std::mt19937(mRand());
    mDistX = std::uniform_int_distribution<int>(0, mDrawRect.w);
    mDistY = std::uniform_int_distribution<int>(0, mDrawRect.h);
    mLifeDist = std::uniform_int_distribution<int>(lifemin, lifemax);
    mVelXDist = std::uniform_real_distribution<float>(velxmin, velxmax);
    mVelYDist = std::uniform_real_distribution<float>(velymin, velymax);
    mVelRDist = std::uniform_real_distribution<float>(rmin, rmax);

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
        if(mParticles[i].mLifetime > 0){
            draw.x = mParticles[i].mPosition.x;
            draw.y = mParticles[i].mPosition.y;
            SDL_SetTextureAlphaMod(mTexture, mColorMod.a);
            SDL_SetTextureColorMod(mTexture, mColorMod.r, mColorMod.g, mColorMod.b);
            SDL_RenderTextureRotated(r, mTexture, nullptr, &draw, mParticles[i].mPosition.z, NULL, SDL_FlipMode::SDL_FLIP_NONE);
        
            mParticles[i].mPosition = mParticles[i].mPosition + mParticles[i].mVelocity;
            mParticles[i].mVelocity = mParticles[i].mVelocity + mParticles[i].mAcceleration;
            mParticles[i].mLifetime--;
        } else {
            mParticles[i].mLifetime = mLifeDist(mGen);
            mParticles[i].mVelocity.x = 0.0f;
            mParticles[i].mVelocity.y = 0.0f;
            mParticles[i].mVelocity.z = 0.0f;
            mParticles[i].mPosition.x = mDrawRect.x + mDistX(mGen);
            mParticles[i].mPosition.y = mDrawRect.y + mDistY(mGen);
            mParticles[i].mPosition.z = 0.0f;
            mParticles[i].mAcceleration.x = mVelXDist(mGen);
            mParticles[i].mAcceleration.y = mVelYDist(mGen);
            mParticles[i].mAcceleration.y = mVelRDist(mGen);
        }
    }
}

}
}
