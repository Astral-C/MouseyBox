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
    if(layer.contains("collidable")) mCollisionEnabled = layer["collidable"].get<bool>();
    if(mLayerShift == 0) mLayerShift = 1;
    for(auto tile : layer["tiles"]){
        mTiles.push_back(tile);
    }
}

TileMapLayer::~TileMapLayer(){
}

void TileMapLayer::Draw(SDL_Renderer* r, Camera* cam){
    if(mMap == nullptr) return;

    float shift_x = static_cast<float>(cam->mRect.x) * mLayerShift;
    float shift_y = static_cast<float>(cam->mRect.y) * mLayerShift;

    int tileSize = mMap->GetTileSize();
    int tilesetPitch = mMap->GetTilesetPitch();

    int tw = static_cast<int>(ceil((float)cam->mRect.w / (float)tileSize)) + 2;
    int th = static_cast<int>(ceil((float)cam->mRect.h / (float)tileSize)) + 2;

    int txStart = (((cam->mRect.x * mLayerShift) / mScale) / tileSize)-1;
    int tyStart = (((cam->mRect.y * mLayerShift) / mScale) / tileSize)-1;

    int renderCallCount = 0;
    for(int y = tyStart; y < tyStart + th; y++){
        if(y < 0 || y >= mMap->GetTileHeight()) continue;
        int ty = y * tileSize;
        for(int x = txStart; x < txStart + tw; x++){
            int tx = x * tileSize;
            if(x < 0 || x >= mMap->GetTileWidth()) continue;

            int tile = mTiles[(y * mMap->GetTileWidth()) + x];
            int tid = TileMap::TILE_IDX(tile);
            if(tid == -1) continue;

            SDL_FRect tileDest {tx * mScale, ty * mScale, tileSize * mScale, tileSize * mScale};
            tileDest.x = std::round(tileDest.x - shift_x);
            tileDest.y = std::round(tileDest.y - shift_y);

            SDL_FRect tileSource {static_cast<float>(std::floor((tid % tilesetPitch) * tileSize)), static_cast<float>(std::floor((tid / tilesetPitch) * tileSize)), static_cast<float>(tileSize), static_cast<float>(tileSize)};
            SDL_RenderTextureRotated(r, mMap->GetTileset(), &tileSource, &tileDest, 0.0f, nullptr, SDL_FlipMode(TileMap::TILE_FLIP_X(tile) | TileMap::TILE_FLIP_Y(tile)));
        }
    }
}

TileMap::TileMap(){}

TileMap::~TileMap(){
    if(mTileSet != nullptr){
        SDL_DestroyTexture(mTileSet);
    }
}

TileMap::TileMap(Renderer* r, std::filesystem::path json){
    std::ifstream jsonStream(json.string());
    nlohmann::json tilemapJson = nlohmann::json::parse(jsonStream);

    mName = tilemapJson["name"];
    mTileSize = tilemapJson["tileSize"];
    mTileHeight = tilemapJson["height"];
    mTileWidth = tilemapJson["width"];

    for(auto layer : tilemapJson["layers"]){
        std::weak_ptr<TileMapLayer> newLayer = r->CreateTilemapLayer(layer);
        if(std::shared_ptr<TileMapLayer> locked = newLayer.lock()){
            locked->mMap = this;
            locked->mDrawRect = {0, 0, static_cast<float>(mTileWidth * mTileSize), static_cast<float>(mTileHeight * mTileSize)};
            mLayers.push_back(newLayer);
        }
    }

    // set for each layer mMapRect = {0, 0, mTileWidth * mTileSize, mTileHeight * mTileSize};

    LoadTileset(r->GetInternalRender(), tilemapJson["tileImg"].get<std::string>().c_str(), mTileSize);

    //Update(r->GetInternalRender());
}

TileMap::TileMap(Renderer* r, nlohmann::json& tilemapJson, uint8_t* data, std::size_t size){
    mName = tilemapJson["name"];
    mTileSize = tilemapJson["tileSize"];
    mTileHeight = tilemapJson["height"];
    mTileWidth = tilemapJson["width"];

    for(auto layer : tilemapJson["layers"]){
        std::weak_ptr<TileMapLayer> newLayer = r->CreateTilemapLayer(layer);
        if(std::shared_ptr<TileMapLayer> locked = newLayer.lock()){
            // memory leak on GC.... oops!
            locked->mMap = this;
            locked->mDrawRect = {0, 0, static_cast<float>(mTileWidth * mTileSize), static_cast<float>(mTileHeight * mTileSize)};
            mLayers.push_back(newLayer);
        }
    }

    LoadTilesetFromMemory(r->GetInternalRender(), mTileSize, data, size);

#ifndef __GAMECUBE__
    //Update(r->GetInternalRender());
#endif
}


void TileMap::NewLayer(Renderer* r){
    mLayers.push_back(r->CreateTilemapLayer());
    std::weak_ptr<TileMapLayer> weak_layer = mLayers.back();

    if(std::shared_ptr<TileMapLayer> layer = weak_layer.lock()){
        layer->mTiles.resize(mTileWidth * mTileHeight);
        std::fill(layer->mTiles.begin(), layer->mTiles.end(), -1);
        layer->mDrawRect = {0, 0, static_cast<float>(mTileWidth * mTileSize), static_cast<float>(mTileHeight * mTileSize)};
    }
}


uint32_t TileMap::TileAt(int x, int y, int z){
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()) {
        int tx = (x / layer->mScale) / mTileSize;
        int ty = (y / layer->mScale) / mTileSize;
        return layer->mTiles[(ty * mTileWidth) + tx] & 0x00FFFFFF;
    } else {
        return -1;
    }
}

bool TileMap::SetTileAt(uint32_t tid, int x, int y, int z, bool fx, bool fy, bool ow){
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()) {
        int tx = (x / layer->mScale) / mTileSize;
        int ty = (y / layer->mScale) / mTileSize;

        if(tx < 0 || tx > mTileWidth || ty > mTileHeight || ty < 0 || z < 0 || z > mLayers.size()) return false;
        layer->mTiles[(ty * mTileWidth) + tx] = fx | fy << 1 | ow << 2 |  static_cast<uint32_t>(tid) << 8;
        return true;
    } else {
        return false;
    }
}

bool TileMap::SetTile(uint32_t tid, int x, int y, int z, bool fx, bool fy, bool ow){
    if(x < 0 || x >= mTileWidth || y >= mTileHeight || y < 0 || z < 0 || z > mLayers.size()) return false;
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()){
        layer->mTiles[(y * mTileWidth) + x] = fx | fy << 1 | ow << 2 | static_cast<uint32_t>(tid) << 8;
    } else {
        return false;
    }
    return true;
}

uint32_t TileMap::GetTile(int x, int y, int z){
    if(x >= mTileWidth || x < 0 || y >= mTileHeight || y < 0 || z >= mLayers.size() || z < 0) return 0x00FFFFFF;
    if(std::shared_ptr<TileMapLayer> layer = mLayers[z].lock()){
        return layer->mTiles[(y * mTileWidth) + x] >> 8;
    } else {
        return  0x00FFFFFF;
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
}

void TileMap::SetSize(int w, int h){
    int ow = mTileWidth;
    int oh = mTileHeight;

    mTileWidth = w;
    mTileHeight = h;

    for(auto layer_weak : mLayers){
        if(std::shared_ptr<TileMapLayer> layer = layer_weak.lock()){
            layer->mTiles.resize(w * h);
            layer->mDrawRect = {0, 0, static_cast<float>(mTileWidth * mTileSize), static_cast<float>(mTileHeight * mTileSize)};

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
   std::size_t bytes_per_row = (std::size_t)w * bytes_per_pixel;
   stbi_uc temp[2048];
   stbi_uc *bytes = (stbi_uc *)image;

   for (row = 0; row < (h>>1); row++) {
      stbi_uc *row0 = bytes + row*bytes_per_row;
      stbi_uc *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      std::size_t bytes_left = bytes_per_row;
      while (bytes_left) {
        std::size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
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
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mTileSetWidth, mTileSetHeight, SDL_GetPixelFormatForMasks(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF), imgData, mTileSetWidth*4);
#else
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mTileSetWidth, mTileSetHeight, SDL_GetPixelFormatForMasks(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000), imgData, mTileSetWidth*4);
#endif
    mTileSet = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureScaleMode(mTileSet, SDL_SCALEMODE_NEAREST);

    mTileSetPitch = mTileSetWidth / mTileSize;

    stbi_image_free(imgData);

    SDL_DestroySurface(surface);
}

void TileMap::LoadTilesetFromMemory(SDL_Renderer* r, int tileSize, uint8_t* data, std::size_t size){
    mTileSize = tileSize;

    int comp;
    unsigned char* imgData = stbi_load_from_memory(data, size, &mTileSetWidth, &mTileSetHeight, &comp, 4);
#ifdef __GAMECUBE__
    stbi__vertical_flip(imgData, mWidth, mHeight, 4);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mTileSetWidth, mTileSetHeight, SDL_GetPixelFormatForMasks(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF), imgData, mTileSetWidth*4);
#else
    SDL_Surface* surface = SDL_CreateSurfaceFrom(mTileSetWidth, mTileSetHeight, SDL_GetPixelFormatForMasks(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000), imgData, mTileSetWidth*4);
#endif
    mTileSet = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureScaleMode(mTileSet, SDL_SCALEMODE_NEAREST);
    if(mTileSet == nullptr) {
        Log::Debug("Couldn't load tileset image");
        return;
    }

    mTileSetPitch = mTileSetWidth / mTileSize;

    stbi_image_free(imgData);
    Log::Debug("Loaded Tileset Image");

    SDL_DestroySurface(surface);
}

}
