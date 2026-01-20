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

    SDL_Texture* mGlyphAtlas { nullptr };

    float mFontScale { 1.0f };
    int mFontSize { 32 }, mAscent { 32 }, mDescent { 32 }, mLineGap { 5 };
    std::vector<stbtt_packedchar> mGlyphMetrics;

    DynFont(SDL_Renderer* r, std::string fontPath, uint32_t fontSize=32, uint32_t charCount=96);
    ~DynFont();

};


class DynText : public Renderable
{
    friend Renderer;

    float time { 0 };
    std::string mText { "MouseyBox!" };
    std::weak_ptr<DynFont> mFont;
    SDL_Color mColorFG { 0xFF, 0xFF, 0xFF, 0x00 };
    SDL_Color mColorBG { 0x00, 0x00, 0x00, 0x00 };

    void SetText(std::string);

public:
    std::string GetText() { return mText; }

    void Draw(SDL_Renderer*, Camera*) override;

    void SetFont(std::shared_ptr<DynFont> font) { mFont = font; }

    DynText();
    ~DynText();
};

}


#endif
