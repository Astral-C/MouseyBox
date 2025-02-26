#ifndef __MB_CIRCLE_H__
#define __MB_CIRCLE_H__
#include <tuple>
#include <vector>
#include <SDL3/SDL.h>
#include <system/Math.hpp>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;

class Circle : public Renderable
{
private:
    friend Renderer;

    float mRadius { 0 };
    bool mDrawFilled { true };
    int mThickness { 1 };
public:
    void SetThickness(int t) { mThickness = t; } 
    void SetFilled(bool f) { mDrawFilled = f; }
    void Draw(SDL_Renderer*, Camera*) override;
    void SetRadius(float r) { mRadius = r; }

    Circle(int r) { mRadius = r; }

    Circle();
    ~Circle();
};

}


#endif