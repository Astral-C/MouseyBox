#ifndef __MB_TILEMAP_H__
#define __MB_TILEMAP_H__
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <system/json.hpp>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;
class TileMap;

struct TilemapCollision {
    float dist_x;
    float dist_y;
    int32_t tile;
};

class TileMapLayer : public Renderable {
    friend TileMap;
    std::shared_ptr<TileMap> mMap;
    std::vector<int32_t> mTiles {};
public:
    float mColorShift[4] { 1.0f, 1.0f, 1.0f, 1.0f };
    float mLayerShift { 1 };
    
    std::vector<int32_t> GetTiles(){ return mTiles; }

    TileMapLayer();
    TileMapLayer(nlohmann::json);
    ~TileMapLayer();
    
    void Draw(SDL_Renderer*, Camera* cam) override;
};

class TileMap : public std::enable_shared_from_this<TileMap> {
    int mTileSize { 0 };
    int mTileWidth { 0 };
    int mTileHeight { 0 };
    

    int mTileSetWidth { 0 };
    int mTileSetHeight { 0 };
    int mTileSetPitch { 0 };

    SDL_Texture* mTileSet { nullptr };

    std::vector<std::weak_ptr<TileMapLayer>> mLayers; 

public:
    std::string mName;

    bool SetTileAt(int32_t, int, int, int);
    int32_t TileAt(int, int, int);

    bool SetTile(int32_t, int, int, int);
    int32_t GetTile(int, int, int);

    void Update(SDL_Renderer*);

    std::vector<std::weak_ptr<TileMapLayer>>& GetLayers(){ return mLayers; }

    void SetMapScale(int);

    void SetSize(int, int);
    void LoadTileset(SDL_Renderer*, std::filesystem::path, int);
    void LoadTilesetFromMemory(SDL_Renderer*, int, uint8_t*, size_t);

    int GetTileWidth() { return mTileWidth; }
    int GetTileHeight() { return mTileHeight; }
    int GetTileSize() { return mTileSize; }
    int GetTilesetPitch() { return mTileSetPitch; }
    SDL_Texture* GetTileset() { return mTileSet; }
    
    int GetTilesetWidth() { return mTileSetWidth; }
    int GetTilesetHeight() { return mTileSetHeight; }

    void Free(){ mLayers.clear(); }
    void NewLayer(Renderer*);
    
    TileMap(Renderer*, nlohmann::json&, uint8_t*, size_t);
    TileMap(Renderer*, std::filesystem::path);
    TileMap();
    ~TileMap();

};

}

#endif