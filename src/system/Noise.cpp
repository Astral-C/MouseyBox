#include <system/Log.hpp>
#include <system/Noise.hpp>
#include <functional>

namespace mb::Noise {
    Perlin::Perlin(uint32_t seed){
        mSeed = seed;
        mGenerator = std::mt19937(seed);

        for(int i = 0; i<256;i++){
            mPermutations[i+256] = mPermutations[i] = mDistribution(mGenerator);
        }
    }

    inline double fade(double t){
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    inline float gradient(uint8_t hash, float x, float y){
        switch (hash & 15){
            case 0x0: return x;
            case 0x1: return x + y;
            case 0x2: return y;
            case 0x3: return -x + y;
            case 0x4: return -x;
            case 0x5: return -x - y;
            case 0x6: return -y;
            case 0x7: return x - y;
            case 0x8: return x;
            case 0x9: return x + y;
            case 0xA: return y;
            case 0xB: return -x + y;
            case 0xC: return -x;
            case 0xD: return -x - y;
            case 0xE: return -y;
            case 0xF: return x - y;
        default:
            return 0;
        }
    }

    float Perlin::at(float x, float y){
        int x0 = static_cast<int>(std::floor(x)) & 255;
        int y0 = static_cast<int>(std::floor(y)) & 255;
        int x1 = (x0+1)&255;
        int y1 = (y0+1)&255;

        float localX = x - std::floor(x);
        float localY = y - std::floor(y);
        
        float u = fade(localX);
        float v = fade(localY);

        uint8_t aa = mPermutations[mPermutations[x0] + y0];
        uint8_t ab = mPermutations[mPermutations[x0] + y1];
        uint8_t ba = mPermutations[mPermutations[x1] + y0];
        uint8_t bb = mPermutations[mPermutations[x1] + y1];
        
        float v0 = gradient(aa, localX, localY);
        float v1 = gradient(ba, localX - 1, localY);
        float v2 = gradient(ab, localX, localY - 1);
        float v3 = gradient(bb, localX - 1, localY - 1);

        float e0 = mb::Math::Lerp<float>(v0, v1, u);
        float e1 = mb::Math::Lerp<float>(v2, v3, u);
        
        return (mb::Math::Lerp<float>(e0, e1, v) + 1) / 2;
    }


    float Perlin::operator()(float x, float y, int octaves, float persist){
        float total = 0.0f;
        float freq = 1.0f;
        float amp = 1.0f;
        float max = 0.0f;

        for(int i = 0; i < octaves; i++){
            total += at(x * freq, y * freq) * amp;

            max += amp;

            amp *= persist;
            freq *= 2;
        }

        return total / max;
    }
}