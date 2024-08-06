#ifndef __MB_TEXT_H__
#define __MB_TEXT_H__
#include <tuple>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <system/Math.hpp>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;

struct Font {
    TTF_Font* font { nullptr };

    ~Font(){
        if(font != nullptr){
            TTF_CloseFont(font);
        }
    }

};


class Text : public Renderable
{
    friend Renderer;

    std::string mText { "MouseyBox!" };
    SDL_Color mColorFG { 0xFF, 0xFF, 0xFF, 0x00 };
    SDL_Color mColorBG { 0x00, 0x00, 0x00, 0x00 };

    void SetText(SDL_Renderer*, TTF_Font*, std::string, int wrap=-1, int align=TTF_WRAPPED_ALIGN_LEFT);

public:
    std::string GetText() { return mText; }
    
    void Draw(SDL_Renderer*, Camera*) override;

    Text();
    ~Text();
};

}


#endif