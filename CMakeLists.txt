cmake_minimum_required(VERSION 3.8)
project(MouseyBox)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

include_directories(editor/lib/imgui)
include_directories(engine/lib)
include_directories(engine/include)
include_directories(mouseybox ${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE MOUSEY_SRC
    "src/*.cpp"
    "src/*.c"
    "lib/tt/*.c"
    "lib/tt/*.h"
    "include/*.hpp"
    "include/*.h"
)

add_library(mouseybox STATIC ${MOUSEY_SRC})

target_link_libraries(mouseybox SDL2::SDL2 SDL2_ttf::SDL2_ttf)

add_executable(mouseytools "tools/tools.cpp")
target_link_libraries(mouseytools mouseybox SDL2::SDL2 SDL2_ttf::SDL2_ttf)