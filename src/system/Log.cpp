#include <system/Log.hpp>
#include <iostream>
#include <fstream>

namespace mb::Log {

    namespace Config {
        std::string mSrc { "MouseyBox" };
        Level mLevel { Level::Debug };
    }

    void SetLevel(Level l){
        Config::mLevel = l;
    }

    void SetSrc(std::string src){
        Config::mSrc = src;
    }
}
