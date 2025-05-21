#ifndef __MB_GRAPHICS_H__
#define __MB_GRAPHICS_H__
#include "system/json.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_rect.h>
#include <graphics/Text.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/Polygon.hpp>
#include <graphics/TileMap.hpp>
#include <graphics/Rect.hpp>
#include <graphics/Camera.hpp>
#include <graphics/Circle.hpp>
#include <graphics/Particle.hpp>
#include <vector>
#include <string>

namespace mb::Graphics {

class Renderer;

class Window {
private:
    friend Renderer;

    std::string mWindowTitle { "MouseyBox" };
    SDL_Window* mWindow { nullptr };
    SDL_Color mClearColor {0, 0, 0, 0};

public:
    SDL_Window* GetInternalWindow() { return mWindow; }

    void Cleanup();
    SDL_FRect GetSize();

    void SetName(std::string);
    Window(std::string, uint32_t, uint32_t);
    ~Window();
};

class Renderer {
private:
    SDL_Texture* mTexture { nullptr };
    SDL_Renderer* mInternalRender { nullptr };
    std::map<std::string, std::shared_ptr<Font>> mFonts;
    std::map<std::string, std::shared_ptr<Sprite>> mSprites;
    std::map<std::string, std::shared_ptr<TileMap>> mTileMaps;

    int mWidth { 0 }, mHeight { 0};
    std::vector<std::shared_ptr<Renderable>> mRenderables;

public:
    Camera mCamera;

    std::function<void(SDL_Renderer*)> mDraw;
    SDL_Renderer* GetInternalRender() { return mInternalRender; }

    std::vector<std::string> GetRenderBackends();
    void SetRenderBackends(std::string, Window*);

    void SetSize(int, int);
    SDL_FRect GetSize();

    void Initialize(Window*);
    std::shared_ptr<SpriteInstance> InstanceSprite(std::string);
    std::shared_ptr<SpriteInstance> InstanceSprite(std::string, int, int);
    std::shared_ptr<Polygon> CreatePolygon(std::vector<Math::Vec2<float>>);
    std::shared_ptr<Rect> CreateRect(float,float,float,float);
    std::shared_ptr<Circle> CreateCircle(float,float,float);
    std::shared_ptr<ParticleSystem> CreateParticleSystem(std::filesystem::path);
    std::shared_ptr<Text> CreateText(std::string, std::string);
    std::shared_ptr<TileMapLayer> CreateTilemapLayer(nlohmann::json);
    std::shared_ptr<TileMapLayer> CreateTilemapLayer();
    std::shared_ptr<TileMap> GetTilemap(std::string);

    void DeleteTilemap(std::string);
    void DeleteFont(std::string);
    void DeleteSprite(std::string);

    std::shared_ptr<Sprite> GetSprite(std::string sprite) {
        if(mSprites.contains(sprite)) {
            return mSprites[sprite];
        } else {
            return nullptr;
        }
    };

    bool SetText(std::shared_ptr<Text> t, std::string font, std::string text, int wrap=-1, TTF_HorizontalAlignment align=TTF_HORIZONTAL_ALIGN_LEFT);

    void Push(std::shared_ptr<Renderable>);
    void Free(std::shared_ptr<Renderable>);

    bool LoadFont(std::filesystem::path, int, std::string="");
    bool LoadFontFromMemory(std::string, uint8_t*, std::size_t, int);
    bool LoadSprite(std::filesystem::path);
    bool LoadSprite(nlohmann::json);
    bool LoadSprites(std::filesystem::path);
    bool LoadSprites(nlohmann::json);
    bool LoadSpriteSimple(std::string, std::filesystem::path);
    bool LoadSpriteFromMemory(nlohmann::json, uint8_t*, std::size_t);
    std::shared_ptr<TileMap> LoadTilemapFromMeory(nlohmann::json&, uint8_t*, std::size_t);
    std::shared_ptr<TileMap> LoadTilemap(std::filesystem::path);
    std::shared_ptr<TileMap> CreateTilemap(std::string);

    void Sort();
    void Update(float, float);
    void Cleanup();

    Renderer();
    ~Renderer();

};

}
#endif
