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
#include <graphics/Pipeline.hpp>

namespace mb::Graphics {

struct FontResource : GraphicsResource {
    stbtt_fontinfo mFontInfo;
    SDL_Texture* mGlyphAtlas { nullptr };

    float mFontScale { 1.0f };
    int mFontSize { 32 }, mAscent { 32 }, mDescent { 32 }, mLineGap { 5 };
    std::vector<stbtt_packedchar> mGlyphMetrics;

    Font(std::string fontPath, uint32_t fontSize=32, uint32_t charCount=96);
    ~Font();

};

enum DynTextEffect : uint32_t {
    NONE = 0,
    COLOR = 1,
    FLOAT = 2,
    RAINBOW = 4,
    SHAKE = 8,
    CUSTOM = 16
};

struct TextVertex {
    float x, y, z;
    uint32_t mPalIndex;
    uint32_t mEffect;
    float mEffectArg1 { 0.1f };
    float mEffectArg0 { 0 };
};

struct TextUniformBuffer {
    float mTime { 0.0f };
    uint32_t mColorCount { 1 };
    SDL_FColor mPalette[16] = {1.0f };
};


class Text : public RenderInstance {
    std::string mText;
    int mWrapWidth { 0 };


public:
    Text();
    ~Text();
};

}


#endif
