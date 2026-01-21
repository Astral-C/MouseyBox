#ifndef __MB_DYNTEXT_H__
#define __MB_DYNTEXT_H__
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <functional>
#include <tuple>
#include <vector>
#include <memory>
#include <string>
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

enum DynTextEffect : uint32_t {
    NONE,
    COLOR,
    FLOAT,
    RAINBOW,
    SHAKE,
    CUSTOM,
    COUNT
};

struct DynTextChunk {
    uint32_t mEffectArg0 { 5 };
    float mEffectArg1 { 0.1f };
    float mTime { 0.0f };
    DynTextEffect mEffect;
    // still unsure if I want to allow custom effets, seems slow
    //std::function<void(int,float)> mCustomEffect;
    std::string mText;
};


class DynText : public Renderable
{
    friend Renderer;

    std::vector<DynTextChunk> mText;
    std::weak_ptr<DynFont> mFont;
    std::vector<SDL_Color> mPalette;

    void SetText(std::string);


public:
    void AddTextChunk(uint32_t, std::string);
    //std::string GetText() { return mText; }

    void SetPalette(std::vector<SDL_Color> colors) { mPalette = colors; }

    void Draw(SDL_Renderer*, Camera*) override;

    void SetFont(std::shared_ptr<DynFont> font) { mFont = font; }

    DynText();
    ~DynText();
};

}


#endif
