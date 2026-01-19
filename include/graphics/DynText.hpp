#ifndef __MB_DYNTEXT_H__
#define __MB_DYNTEXT_H__
#include <SDL3/SDL_render.h>
#include <tuple>
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include <system/Math.hpp>
#include <system/stb_rect_pack.h>
#include <graphics/stb_truetype.h>
#include <graphics/Renderable.hpp>

namespace mb::Graphics {

class Renderer;

struct DynFont {
    stbtt_fontinfo mFontInfo;
    SDL_Texture* mGlyphAtlas;

    DynFont(SDL_Renderer* r, std::string fontPath, uint32_t fontSize=32, uint32_t charCount=96);
    ~DynFont();

};


class DynText : public Renderable
{
    friend Renderer;

    std::string mText { "MouseyBox!" };
    std::weak_ptr<DynFont> mFont;
    SDL_Color mColorFG { 0xFF, 0xFF, 0xFF, 0x00 };
    SDL_Color mColorBG { 0x00, 0x00, 0x00, 0x00 };

    void SetText(SDL_Renderer*, std::shared_ptr<DynFont>, std::string, int wrap=-1, TTF_HorizontalAlignment align=TTF_HORIZONTAL_ALIGN_LEFT);

public:
    std::string GetText() { return mText; }

    void Draw(SDL_Renderer*, Camera*) override;

    DynText();
    ~DynText();
};

}


#endif
