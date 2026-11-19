#ifndef __MB_CIRCLE_H__
#define __MB_CIRCLE_H__
#include <SDL3/SDL_pixels.h>
#include <tuple>
#include <vector>
#include <SDL3/SDL.h>
#include <system/Math.hpp>
#include <graphics/Pipeline.hpp>

namespace mb::Graphics {

struct CircleInstance : RenderInstance {
    uint8_t DrawFilled { 1 };
    uint8_t Thickness { 1 };
    float Radius { 0 };
};

}


#endif