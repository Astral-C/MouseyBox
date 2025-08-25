#ifndef __MB_UTIL_H__
#define __MB_UTIL_H__

#include <random>

namespace mb::Util {

    constexpr uint32_t Hash(std::string string){
        uint32_t hash = 0;

        for (char c : string)
        {
            hash = ((hash << 8) + c) % 0x30237807;
        }
        return hash;
    }

}

#endif