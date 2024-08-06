#ifndef __MB_LOGGING_H__
#define __MB_LOGGING_H__
#include <string>
#include <format>

namespace mb::Log {

    enum class Level : int {
        Info,
        Warn,
        Error,
        Debug
    };

    namespace Config {
        extern std::string mSrc;
        extern std::ostream* mDest;
        extern Level mLevel;
    }

    void SetLevel(Level);
    void SetSrc(std::string);

    template<typename... Args>
    void Error(std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Error){
            std::string vfmtstr = std::format("[{}][ERROR] {}\n", Config::mSrc, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }

    template<typename... Args>
    void Warn(std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Warn){
            std::string vfmtstr = std::format("[{}][WARN] {}\n", Config::mSrc, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }

    template<typename... Args>
    void Debug(std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Debug){
            std::string vfmtstr = std::format("[{}][DEBUG] {}\n", Config::mSrc, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }
    
    template<typename... Args>
    void Info(std::string fmt_str, Args&&... args){
        std::string vfmtstr = std::format("[{}][INFO] {}\n", Config::mSrc, fmt_str);
        *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
    }

    // explicit src

    template<typename... Args>
    void ErrorFrom(std::string src, std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Error){
            std::string vfmtstr = std::format("[{}][ERROR] {}\n", src, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }

    template<typename... Args>
    void WarnFrom(std::string src, std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Warn){
            std::string vfmtstr = std::format("[{}][WARN] {}\n", src, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }

    template<typename... Args>
    void DebugFrom(std::string src, std::string fmt_str, Args&&... args){
        if(Config::mLevel >= Level::Debug){
            std::string vfmtstr = std::format("[{}][DEBUG] {}\n", src, fmt_str);
            *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
        }
    }
    
    template<typename... Args>
    void InfoFrom(std::string src, std::string fmt_str, Args&&... args){
        std::string vfmtstr = std::format("[{}][INFO] {}\n", src, fmt_str);
        *Config::mDest << std::vformat(vfmtstr, std::make_format_args(args...));
    }
}

#endif