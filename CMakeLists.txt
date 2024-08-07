cmake_minimum_required(VERSION 3.8)
project(MouseyBox)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

include_directories(lib)
include_directories(include)
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

if(EXAMPLE)

file(GLOB_RECURSE EXAMPLE_SRC
    "example/src/*.cpp"
    "example/src/*.c"
    "example/include/*.hpp"
    "example/include/*.h"
)


add_executable(example ${EXAMPLE_SRC})

target_include_directories(example PRIVATE example/include)
target_link_libraries(example mouseybox SDL2::SDL2 SDL2_ttf::SDL2_ttf)

endif()