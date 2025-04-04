#ifndef __MB_PARTICLE_H__
#define __MB_PARTICLE_H__
#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <system/json.hpp>
#include <graphics/Renderable.hpp>
#include <system/Math.hpp>
#include <random>

namespace mb::Graphics {

class Renderer;

struct Particle {
    mb::Math::Vec2<float> mPosition { 0, 0 };
    mb::Math::Vec2<float> mVelocity { 0.0f, -0.015f };
    mb::Math::Vec2<float> mAcceleration { 0.0f, -0.005f };
    int mLifetime { 0 };
};

class ParticleSystem : public Renderable {
    int mParticleMax { 0 };
    int mWidth { 0 }, mHeight { 0 };
    std::random_device mRand {};
    std::mt19937 mGen {};
    std::uniform_int_distribution<int> mDistX{};
    std::uniform_int_distribution<int> mDistY{};
    std::uniform_int_distribution<int> mLifeDist{};
    std::uniform_real_distribution<float> mVelXDist{};
    std::uniform_real_distribution<float> mVelYDist{};
    int mTextureWidth { 0 }, mTextureHeight { 0 };
    SDL_Texture* mTexture { nullptr };
    Particle* mParticles { nullptr };

public:
    void Draw(SDL_Renderer*, Camera*);
    SDL_Texture* GetTexture() { return mTexture; }

    int GetTextureWidth() { return mTextureWidth; }
    int GetTextureHeight() { return mTextureHeight; }

    int GetWidth() { return mWidth; }
    int GetHeight() { return mHeight; }
    
    ParticleSystem(SDL_Renderer*, nlohmann::json&);
    ParticleSystem(SDL_Renderer*, nlohmann::json&, uint8_t*, std::size_t);
    ParticleSystem();
    ~ParticleSystem();
};

}

#endif
