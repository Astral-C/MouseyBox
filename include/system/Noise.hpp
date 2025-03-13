#ifndef __MB_NOISE_H__
#define __MB_NOISE_H__

#include <random>
#include <system/Math.hpp>

namespace mb::Noise {

    class Perlin {
        uint32_t mSeed {0};
        uint8_t mPermutations[512];

        std::mt19937 mGenerator;
        std::uniform_int_distribution<uint8_t> mDistribution {};
        float at(float x, float y);

    public:
        float operator()(float x, float y, int octaves=4, float persist=0.5f);
        Perlin(uint32_t seed);
        ~Perlin(){}
    };
}

#endif