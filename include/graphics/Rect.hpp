#ifndef __MB_RECT_H__
#define __MB_RECT_H__
#include <tuple>
#include <vector>
#include <SDL2/SDL.h>
#include <system/Math.hpp>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;

class Rect : public Renderable
{
private:
    friend Renderer;
    bool mDrawFilled { true };
    int mThickness { 1 };
public:
    void SetThickness(int t) { mThickness = t; } 
    void SetFilled(bool f) { mDrawFilled = f; }
    void Draw(SDL_Renderer*, Camera*) override;

    Rect();
    ~Rect();
};

}


#endif