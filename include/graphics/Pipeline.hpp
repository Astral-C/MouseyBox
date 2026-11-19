#ifndef __MB_RENDERABLE_H__
#define __MB_RENDERABLE_H__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_rect.h>
#include <functional>
#include <graphics/RenderableType.hpp>
#include <concepts>
#include <cstdint>
#include <vector>
#include <map>
#include <string>

namespace mb::Graphics {

struct RenderInstance {
    RenderableType type;
    float x, y, z;
    uint16_t Outline { 0 };
    uint16_t DropShadow { 0 };
    uint16_t ShadowX { 0 }, ShadowY { 0 };
    uint32_t ShadowSpread { 0 };
    SDL_Color ShadowColor;
    SDL_Color OutlineColor;
};

struct GraphicsResource {
    std::string Name;
};

class PipelineRenderer {
    protected:
        SDL_GPUGraphicsPipeline* mPipeline;
        SDL_GPUShader* mVertexShader;
        SDL_GPUShader* mFragmentShader;
        
    public:
        void Submit();
};

template <typename T>
requires std::derived_from<T, RenderInstance>
class GenericRenderer {
    private:
        std::vector<T> mInstances;
    public:
        void Submit();
};

template <typename T, typename U>
requires std::derived_from<T, GraphicsResource> && std::derived_from<U, RenderInstance>
class ResourceRenderer {
    private:
        std::map<T, std::vector<U>> mInstances; 
    public:
        void Submit();
};

}

template <>
struct std::hash<mb::Graphics::GraphicsResource> {
  std::size_t operator()(const mb::Graphics::GraphicsResource& k) const {
    return hash<string>()(k.Name);
  }
};

#endif
