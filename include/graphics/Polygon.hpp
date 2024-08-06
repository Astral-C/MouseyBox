#ifndef __MB_POLYGON_H__
#define __MB_POLYGON_H__
#include <tuple>
#include <vector>
#include <SDL2/SDL.h>
#include <system/Math.hpp>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;

class Polygon : public Renderable
{
private:
    friend Renderer;

public:
    std::vector<Math::Vec2<float>> mPoints {};
    void Draw(SDL_Renderer*, Camera*) override;

    bool IsColliding(Polygon&);

    Polygon();
    ~Polygon();
};

}


#endif