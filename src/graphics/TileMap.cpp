#include <system/json.hpp>
#include <system/Log.hpp>
#include <graphics/Graphics.hpp>
#include <graphics/TileMap.hpp>
#include <graphics/stb_image.h>
#include <filesystem>
#include <fstream>
#include <format>
#include <set>

namespace mb::Graphics {

TileMapLayer::TileMapLayer(){
    mType = RenderableType::TilemapLayer;
}

TileMapLayer::TileMapLayer(nlohmann::json layer){
    mType = RenderableType::TilemapLayer;
    mPriority = layer["priority"];
    if(layer.contains("parallaxFactor")) mLayerShift = layer["parallaxFactor"];
    if(layer.contains("lightness")){
        mColorShift[0] = layer["lightness"][0];
        mColorShift[1] = layer["lightness"][1];
        mColorShift[2] = layer["lightness"][2];
        mColorShift[3] = layer["lightness"][3];
    }
    if(mLayerShift == 0) mLayerShift = 1;
    for(auto tile : layer["tiles"]){
        mTiles.push_back(tile);
    }
}

TileMapLayer::~TileMapLayer(){
    if(mTexture != nullptr){
        SDL_DestroyTexture(mTexture);
    }
}

void TileMapLayer::Draw(SDL_Renderer* r, Camera* cam){
#ifndef __GAMECUBE__
    SDL_FRect shifted = {
        static_cast<float>(mDrawRect.x),
        static_cast<float>(mDrawRect.y),
        static_cast<float>(mDrawRect.w * mScale),
        static_cast<float>(mDrawRect.h * mScale)
    };

    shifted.x = (shifted.x - static_cast<float>(cam->mRect.x)) * mLayerShift;
    shifted.y = (shifted.y - static_cast<float>(cam->mRect.y)) * mLayerShift;

    SDL_RenderCopyF(r, mTexture, NULL, &shifted);
#else
    float shift_x = static_cast<float>(cam->mRect.x) * mLayerShift;
    float shift_y = static_cast<float>(cam->mRect.y) * mLayerShift;

    int tileSize = mMap->GetTileSize();
    int tilesetPitch = mMap->GetTilesetPitch();

    int tw = static_cast<int>(ceil((float)cam->mRect.w / (float)tileSize));
    int th = static_cast<int>(ceil((float)cam->mRect.h / (float)tileSize));

    int txStart = (((cam->mRect.x * mLayerShift) / mScale) / tileSize);
    int tyStart = (((cam->mRect.y * mLayerShift) / mScale) / tileSize);

    int renderCallCount = 0;
    for(int y = tyStart; y < tyStart + th; y++){
        if(y < 0 || y > mMap->GetTileHeight()) continue;
        int ty = y * tileSize;
        for(int x = txStart; x < txStart + tw; x++){
            int tx = x * tileSize;
            if(x < 0 || x > mMap->GetTileWidth()) continue;
            
            int tid = mTiles[(y * mMap->GetTileWidth()) + x];
            if(tid == -1) continue;

            SDL_FRect tileDest {(tx * mScale), (ty * mScale), tileSize * mScale, tileSize * mScale};
            tileDest.x -= shift_x;
            tileDest.y -= shift_y;

            SDL_Rect tileSource {(tid % tilesetPitch) * tileSize, (tid / tilesetPitch) * tileSize, tileSize, tileSize};
            SDL_RenderCopyF(r, mMap->GetTileset(), &tileSource, &tileDest);
            renderCallCount++;
        }
    }
    mb::Log::Debug(std::format("Rendering Layer took {} render calls", renderCallCount));

#endif
}

TileMap::TileMap(){}

TileMap::~TileMap(){
    if(mTileSet != nullptr){
        SDL_DestroyTexture(mTileSet);
    }
}

TileMap::TileMap(Renderer* r, std::filesystem::path json){
    std::fstream jsonStream(json.string());
    nlohmann::json tilemapJson = nlohmann::json::parse(jsonStream);

    mName = tilemapJson["name"];
    mTileSize = tilemapJson["tileSize"];
    mTileHeight = tilemapJson["height"];
    mTileWidth = tilemapJson["width"];

    for(auto layer : tilemapJson["layers"]){
        std::weak_ptr<TileMapLayer> newLayer = r->CreateTilemapLayer(layer);
        if(std::shared_ptr<TileMapLayer> locked = newLayer.lock()){
            locked->mDrawRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};
            mLayers.push_back(newLayer);
        }
    }

    // set for each layer mMapRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};

    LoadTileset(r->GetInternalRender(), tilemapJson["tileImg"].get<std::string>().c_str(), mTileSize);

    Update(r->GetInternalRender());
}

TileMap::TileMap(Renderer* r, nlohmann::json& tilemapJson, uint8_t* data, size_t size){
    mName = tilemapJson["name"];
    mTileSize = tilemapJson["tileSize"];
    mTileHeight = tilemapJson["height"];
    mTileWidth = tilemapJson["width"];

    for(auto layer : tilemapJson["layers"]){
        std::weak_ptr<TileMapLayer> newLayer = r->CreateTilemapLayer(layer);
        if(std::shared_ptr<TileMapLayer> locked = newLayer.lock()){
#ifdef __GAMECUBE__
            // memory leak on GC.... oops!
            locked->mMap = std::shared_ptr<TileMap>(this);
#endif
            locked->mDrawRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};
            mLayers.push_back(newLayer);
        }
    }

    LoadTilesetFromMemory(r->GetInternalRender(), mTileSize, data, size);

#ifndef __GAMECUBE__
    Update(r->GetInternalRender());
#endif
}


void TileMap::NewLayer(Renderer* r){
    mLayers.push_back(r->CreateTilemapLayer());
    std::weak_ptr<TileMapLayer> weak_layer = mLayers.back();
    
    if(std::shared_ptr<TileMapLayer> layer = weak_layer.lock()){
        layer->mTiles.resize(mTileWidth * mTileHeight);
        std::fill(layer->mTiles.begin(), layer->mTiles.end(), -1);
        layer->mDrawRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};
    }
}


int32_t TileMap::TileAt(int x, int y, int z){
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()) {        
        int tx = (x / layer->mScale) / mTileSize;
        int ty = (y / layer->mScale) / mTileSize;
        return layer->mTiles[(ty * mTileWidth) + tx];
    } else {
        return -1;
    }
}

bool TileMap::SetTileAt(int32_t tid, int x, int y, int z){
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()) {
        int tx = (x / layer->mScale) / mTileSize;
        int ty = (y / layer->mScale) / mTileSize;

        if(tx < 0 || tx > mTileWidth || ty > mTileHeight || ty < 0 || z < 0 || z > mLayers.size()) return false;
        
        layer->mTiles[(ty * mTileWidth) + tx] = tid;
        return true;
    } else {
        return false;
    }
}

bool TileMap::SetTile(int32_t tid, int x, int y, int z){
    if(x < 0 || x > mTileWidth || y > mTileHeight || y < 0 || z < 0 || z > mLayers.size()) return false;
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()){
        layer->mTiles[(y * mTileWidth) + x] = tid;
    } else {
        return false;
    }
    return true;
}

int32_t TileMap::GetTile(int x, int y, int z){
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()){
        return layer->mTiles[(y * mTileWidth) + x];
    } else {
        return -1;
    }
}

void TileMap::SetMapScale(int scale){
    for(auto layer_weak : mLayers){
        if(std::shared_ptr<TileMapLayer> layer = layer_weak.lock()){
            layer->SetScale(scale);
        }
    }
}

void TileMap::Update(SDL_Renderer* r){
    for(auto layer_weak : mLayers){
        if(std::shared_ptr<TileMapLayer> layer = layer_weak.lock()){
            if(layer->mTexture == nullptr){
                Log::Debug("Creating Layer Texture");

                layer->mTexture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTileWidth * mTileSize, mTileHeight * mTileSize);
                if(layer->mTexture == nullptr){
                    Log::Error(std::format("Error Creating Layer Texture: {}", SDL_GetError()));
                }
            }

            SDL_SetRenderTarget(r, layer->mTexture);
            SDL_RenderClear(r);

            if(SDL_SetTextureBlendMode(layer->mTexture, SDL_BLENDMODE_BLEND) < 0){
                mb::Log::Error("MouseyBox", std::format("Error Setting Blend Mode: {}", SDL_GetError()));
            }

            for(int y = 0; y < mTileHeight; y++){
                int ty = y * mTileSize;
                for(int x = 0; x < mTileWidth; x++){
                    int tx = x * mTileSize;
                    int tid = layer->mTiles[(y * mTileWidth) + x];
                    if(tid == -1) continue;

                    SDL_Rect tileDest {tx, ty, mTileSize, mTileSize};         
                    SDL_Rect tileSource {(tid % mTileSetPitch) * mTileSize, (tid / mTileSetPitch) * mTileSize, mTileSize, mTileSize};

                    SDL_RenderCopy(r, mTileSet, &tileSource, &tileDest);
                }   
            }
            SDL_SetTextureColorMod(layer->mTexture, static_cast<uint8_t>(0xFF * layer->mColorShift[0]), static_cast<uint8_t>(0xFF * layer->mColorShift[1]), static_cast<uint8_t>(0xFF * layer->mColorShift[2]));
            SDL_SetTextureAlphaMod(layer->mTexture, static_cast<uint8_t>(0xFF * layer->mColorShift[3]));
        }
    }

    SDL_SetRenderTarget(r, nullptr);
}

void TileMap::SetSize(int w, int h){
    int ow = mTileWidth;
    int oh = mTileHeight;

    mTileWidth = w;
    mTileHeight = h;

    for(auto layer_weak : mLayers){
        if(std::shared_ptr<TileMapLayer> layer = layer_weak.lock()){
            layer->mTiles.resize(w * h);
            layer->mDrawRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};
            
            for(int y = 0; y < mTileHeight; y++){
                for(int x = 0; x < mTileWidth; x++){
                    if(y > oh || x > ow){
                        layer->mTiles[y * mTileWidth + x] = -1;
                    }
                }
            }
        }
    }
}

// From STB image
static void stbi__vertical_flip(void *image, int w, int h, int bytes_per_pixel)
{
   int row;
   size_t bytes_per_row = (size_t)w * bytes_per_pixel;
   stbi_uc temp[2048];
   stbi_uc *bytes = (stbi_uc *)image;

   for (row = 0; row < (h>>1); row++) {
      stbi_uc *row0 = bytes + row*bytes_per_row;
      stbi_uc *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      size_t bytes_left = bytes_per_row;
      while (bytes_left) {
         size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
         memcpy(temp, row0, bytes_copy);
         memcpy(row0, row1, bytes_copy);
         memcpy(row1, temp, bytes_copy);
         row0 += bytes_copy;
         row1 += bytes_copy;
         bytes_left -= bytes_copy;
      }
   }
}


void TileMap::LoadTileset(SDL_Renderer* r, std::filesystem::path p, int tileSize){
    mTileSize = tileSize;

    int comp;    
    unsigned char* imgData = stbi_load(p.string().c_str(), &mTileSetWidth, &mTileSetHeight, &comp, 4);

#ifdef __GAMECUBE__
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mTileSetWidth, mTileSetHeight, 32, mTileSetWidth*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mTileSetWidth, mTileSetHeight, 32, mTileSetWidth*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
    mTileSet = SDL_CreateTextureFromSurface(r, surface);

    mTileSetPitch = mTileSetWidth / mTileSize;

    stbi_image_free(imgData);

    SDL_FreeSurface(surface);
}

void TileMap::LoadTilesetFromMemory(SDL_Renderer* r, int tileSize, uint8_t* data, size_t size){
    mTileSize = tileSize;

    int comp;
    unsigned char* imgData = stbi_load_from_memory(data, size, &mTileSetWidth, &mTileSetHeight, &comp, 4);
#ifdef __GAMECUBE__
    stbi__vertical_flip(imgData, mTileSetWidth, mTileSetHeight, 4);
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mTileSetWidth, mTileSetHeight, 32, mTileSetWidth*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(imgData, mTileSetWidth, mTileSetHeight, 32, mTileSetWidth*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
    mTileSet = SDL_CreateTextureFromSurface(r, surface);
    if(mTileSet == nullptr) {
        Log::Debug("Couldn't load tileset image");
        return;
    }

    mTileSetPitch = mTileSetWidth / mTileSize;

    stbi_image_free(imgData);
    Log::Debug("Loaded Tileset Image");

    SDL_FreeSurface(surface);
}

}