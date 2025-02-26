cmake_minimum_required(VERSION 3.8)
project(MouseyBox)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

include_directories(lib)
include_directories(include)
include_directories(lib/asio/asio/include)
include_directories(mouseybox ${SDL3_INCLUDE_DIRS})

file(GLOB_RECURSE MOUSEY_SRC
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
    "include/*.h"
)

add_library(mouseybox STATIC ${MOUSEY_SRC})

target_link_libraries(mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf)

add_executable(mouseytools "tools/tools.cpp")
target_link_libraries(mouseytools mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf)

if(EXAMPLE)

file(GLOB_RECURSE EXAMPLE_SRC
    "example/src/*.cpp"
    "example/src/*.c"
    "example/include/*.hpp"
    "example/include/*.h"
)


add_executable(example ${EXAMPLE_SRC})

target_include_directories(example PRIVATE example/include)
target_link_libraries(example mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf)

endif()