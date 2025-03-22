#include <system/Log.hpp>
#include <graphics/Graphics.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/stb_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <filesystem>
#include <format>
#include <fstream>

#ifdef __SWITCH__
#include <switch.h>
#endif

namespace mb {
namespace Graphics {

Window::Window(std::string name){
    mb::Log::InfoFrom("MouseyBox", "Creating Window");
    mWindowTitle = name;
#ifdef __SWITCH__
    mWindow = SDL_CreateWindow(mWindowTitle.data(), 1280, 720, 0);
#elif __GAMECUBE__
    mWindow = SDL_CreateWindow(mWindowTitle.data(), 640, 480, 0);
#else
    mWindow = SDL_CreateWindow(mWindowTitle.data(), 1280, 720, SDL_WINDOW_RESIZABLE);
#endif

    if(mWindow == nullptr){
        Log::Error("Error creating SDL Window {}", SDL_GetError());
        SDL_Quit();
    }
}

Window::~Window(){}

void Window::SetName(std::string name){
    SDL_SetWindowTitle(mWindow, name.data());
}

void Window::Cleanup(){
    SDL_DestroyWindow(mWindow);
}

Renderer::Renderer(){}
Renderer::~Renderer(){}

void Renderer::Initialize(Window* win){
    mb::Log::InfoFrom("MouseyBox", "Initing Renderer");
    if(!win && !win->mWindow){
        Log::Error("No Window on Renderer Init!");
        return;
    }

    if(TTF_Init() < 0){
        Log::Error("Unable to init SDL_ttf! Text rendering won't work");
    }

    const char* driver = nullptr;
    for(int i = 0; i < SDL_GetNumRenderDrivers(); i++){
        if(driver == nullptr && strcmp(SDL_GetRenderDriver(i), "opengl") == 0){
            driver = "opengl";
        }
        if(driver == nullptr && strcmp(SDL_GetRenderDriver(i), "opengles") == 0){
            driver = "opengles";
        }
        //if(strcmp(SDL_GetRenderDriver(i), "vulkan") == 0){
        //    driver = "vulkan"; // prefer vulkan
        //    break;
        //}
    }
    if(driver == nullptr){
        mb::Log::DebugFrom("MouseyBox", "No Render Drivers Found");
        SDL_Quit();
    }

    mb::Log::DebugFrom("MouseyBox", "Using driver {}", driver);

    mInternalRender = SDL_CreateRenderer(win->mWindow, driver);

    if(mInternalRender == nullptr){
        Log::InfoFrom("MouseyBox", "Error Creating SDL Rendererer: {}", SDL_GetError());
        SDL_Quit();
    }
    mb::Log::InfoFrom("MouseyBox", "Initing Camera");
    mCamera.Init(mInternalRender);
    mb::Log::InfoFrom("MouseyBox", "Renderer Initizlized");
}

SDL_FRect Renderer::GetSize(){
    int w, h;
    SDL_GetCurrentRenderOutputSize(mInternalRender, &w, &h);
    SDL_FRect rendererRect {0, 0, static_cast<float>(w), static_cast<float>(h)};
    return rendererRect;
}

bool Renderer::LoadSprites(std::filesystem::path path){
    nlohmann::json spriteConfig = nlohmann::json::parse(std::ifstream(path.string()));
    mb::Log::Debug("Parsed sprite config!");

    for(auto sprite : spriteConfig){
        if(mSprites.contains(sprite["name"])) continue;
        
        std::shared_ptr<Sprite> newSprite = std::make_shared<Sprite>(mInternalRender, sprite);
        Log::Debug("Loading Sprite {}", sprite["name"].get<std::string>());
        mSprites.insert({sprite["name"], newSprite});
    }

    return true;
}

bool Renderer::LoadSprite(std::filesystem::path path){
    nlohmann::json spriteConfig = nlohmann::json::parse(std::ifstream(path.string()));
    mb::Log::Debug("Parsed sprite config!");
    
    if(mSprites.contains(spriteConfig["name"])) return true;

    std::shared_ptr<Sprite> newSprite = std::make_shared<Sprite>(mInternalRender, spriteConfig);
    Log::Debug("Loading Sprite {}", spriteConfig["name"].get<std::string>());
    mSprites.insert({spriteConfig["name"], newSprite});

    return true;
}

bool Renderer::LoadSpriteFromMemory(nlohmann::json spriteConfig, uint8_t* data, size_t size){
    if(mSprites.contains(spriteConfig["name"])) return true;

    std::shared_ptr<Sprite> newSprite = std::make_shared<Sprite>(mInternalRender, spriteConfig, data, size);
    Log::Debug("Loading Sprite {}", spriteConfig["name"].get<std::string>());
    mSprites.insert({spriteConfig["name"], newSprite});

    return true;
}

std::shared_ptr<Rect> Renderer::CreateRect(float x, float y, float w, float h){
    std::shared_ptr<Rect> rect = std::make_shared<Rect>();

    (*rect->GetRect()) = { x, y, w, h };

    mRenderables.push_back(rect);
    return rect;
}

std::shared_ptr<Circle> Renderer::CreateCircle(float x, float y, float r){
    std::shared_ptr<Circle> c = std::make_shared<Circle>();

    (*c->GetRect()) = { x, y, r, r };

    c->SetRadius(r);

    mRenderables.push_back(c);
    return c;
}

std::shared_ptr<Polygon> Renderer::CreatePolygon(std::vector<Math::Vec2<float>> points){
    std::shared_ptr<Polygon> poly = std::make_shared<Polygon>();
    for(auto point : points){
        poly->mPoints.push_back(point);
    }

    mRenderables.push_back(poly);
    return poly;
}

std::shared_ptr<SpriteInstance> Renderer::InstanceSprite(std::string name){
    if(mSprites.count(name) != 0){
        std::shared_ptr<SpriteInstance> nSprite = std::make_shared<SpriteInstance>(mSprites[name]);
        mRenderables.push_back(nSprite);
        return nSprite;
    } else {
        return nullptr;
    }
}

std::shared_ptr<SpriteInstance> Renderer::InstanceSprite(std::string name, int x, int y){
    if(mSprites.count(name) != 0){
        std::shared_ptr<SpriteInstance> nSprite = std::make_shared<SpriteInstance>(mSprites[name]);
        
        nSprite->GetRect()->x = x;
        nSprite->GetRect()->y = y;

        mRenderables.push_back(nSprite);
        
        return nSprite;
    } else {
        return nullptr;
    }
}

void Renderer::Push(std::shared_ptr<Renderable> r){
    mRenderables.push_back(r);
}

void Renderer::Free(std::shared_ptr<Renderable> r){
    std::erase(mRenderables, r);
}


bool Renderer::LoadFont(std::filesystem::path path, int ptSize, std::string name){
    TTF_Font* ttf = TTF_OpenFont(path.c_str(), ptSize);
    
    if(ttf == nullptr) return false;

    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->font = ttf;
    if(name == ""){
        mFonts.insert({path, font});
    } else {
        mFonts.insert({name, font});
    }
    return true;
}

bool Renderer::LoadFontFromMemory(std::string name, uint8_t* data, size_t size, int ptSize){
    SDL_IOStream* mem = SDL_IOFromMem(data, size);
    TTF_Font* ttf = TTF_OpenFontIO(mem, 1, ptSize);
    
    if(ttf == nullptr) return false;

    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->font = ttf;
    mFonts.insert({name, font});
    
    return true;
}

std::shared_ptr<Text> Renderer::CreateText(std::string font, std::string text){
    if(mFonts.count(font) != 0){
        std::shared_ptr<Text> newText = std::make_shared<Text>();
        mRenderables.push_back(newText);
        newText->SetText(mInternalRender, mFonts.at(font)->font, text);
        return newText;
    } else {
        return nullptr;
    }
}

bool Renderer::SetText(std::shared_ptr<Text> t, std::string font, std::string text, int wrap, TTF_HorizontalAlignment align){
    if(mFonts.count(font) != 0){
        t->SetText(mInternalRender, mFonts.at(font)->font, text, wrap, align);
        return true;
    } else {
        return false;
    }
}

std::shared_ptr<TileMapLayer> Renderer::CreateTilemapLayer(nlohmann::json layerJson){
    std::shared_ptr<TileMapLayer> layer = std::make_shared<TileMapLayer>(layerJson);
    mRenderables.push_back(layer);
    return layer;
}

std::shared_ptr<TileMapLayer> Renderer::CreateTilemapLayer(){
    std::shared_ptr<TileMapLayer> layer = std::make_shared<TileMapLayer>();
    mRenderables.push_back(layer);
    return layer;
}

std::shared_ptr<TileMap> Renderer::CreateTilemap(std::string name){
    if(mTileMaps.count(name) > 0){
        mTileMaps.erase(name);
    }
    
    std::shared_ptr<TileMap> map = std::make_shared<TileMap>();
    map->mName = name;
    mTileMaps.insert({map->mName, map});
    return map;
}

void Renderer::DeleteTilemap(std::string name){
    //TODO: Clear layers associated with this tilemap?
    for(auto layer : mTileMaps[name]->GetLayers()){
        Free(layer.lock());
    }
    mTileMaps.erase(name);
}

void Renderer::DeleteSprite(std::string name){
    Log::Debug("Deleting Sprite {}", name);
    mSprites.erase(name);
}

std::shared_ptr<TileMap> Renderer::LoadTilemap(std::filesystem::path path){
    std::shared_ptr<TileMap> map = std::make_shared<TileMap>(this, path);
    mTileMaps.insert({map->mName, map});
    return map;
}

std::shared_ptr<TileMap> Renderer::LoadTilemapFromMeory(nlohmann::json& mapJson, uint8_t* data, size_t size){
    std::shared_ptr<TileMap> map = std::make_shared<TileMap>(this, mapJson, data, size);
    mTileMaps.insert({map->mName, map});
    return map;
}

std::shared_ptr<TileMap> Renderer::GetTilemap(std::string name){
    if(mTileMaps.count(name) != 0){
        return mTileMaps.at(name);
    }
    return nullptr;
}

void Renderer::Sort(){
    std::sort(mRenderables.begin(), mRenderables.end(), [](std::shared_ptr<Renderable> lhs, std::shared_ptr<Renderable> rhs){ return lhs->GetPrio() < rhs->GetPrio(); });
}

void Renderer::Update(){
    SDL_RenderClear(mInternalRender);
    Sort();

    mCamera.Update(mInternalRender);

    for(auto r : mRenderables){
        if(r->IsVisible()) r->Draw(mInternalRender, &mCamera);
    }

    if(mDraw){
        mDraw(mInternalRender);
    }

    SDL_RenderPresent(mInternalRender);
}


void Renderer::Cleanup(){
    mFonts.clear();
    mSprites.clear();
    mTileMaps.clear();
    mRenderables.clear();
    if(mInternalRender != nullptr){
        SDL_DestroyRenderer(mInternalRender);
    }
    TTF_Quit();
}

}
}
