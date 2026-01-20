cmake_minimum_required(VERSION 3.10)
project(MouseyBox)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)


option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(ENABLE_OPENMPT "Enable OpenMPT in Audio" OFF)
option(ENABLE_XM "Enable libxm XM playables in Audio" OFF)
option(DISABLE_ENTITY "Disalbe Built in Entity System" OFF)

if(DISABLE_ENTITY)
    add_compile_definitions(DISABLE_ENTITY)
endif()

if(NOT EMSCRIPTEN)
    if(ENABLE_OPENMPT)
        add_compile_definitions(ENABLE_OPENMPT)
    endif()

    if(ENABLE_XM)
        add_compile_definitions(ENABLE_XM)
        add_subdirectory(lib/libxm/src libxm_build)
    endif()


    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
endif()

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

if(EMSCRIPTEN)
    # requires that you have sdl tt3 port py in this folder! should be fixed once this gets merged in emscripten upstream
    target_compile_options(mouseybox PRIVATE --use-port=/home/leaf/Downloads/sdl3_ttf.py)
    target_link_options(mouseybox PRIVATE --use-port=/home/leaf/Downloads/sdl3_ttf.py)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    target_link_libraries(mouseybox)
else()
    target_link_libraries(mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf)
endif()


if(ENABLE_OPENMPT)
    target_link_libraries(mouseybox openmpt)
endif()

if(ENABLE_XM)
    target_link_libraries(mouseybox xm)
endif()

if(TOOLS)
    add_executable(mouseytools "tools/tools.cpp")
    target_link_libraries(mouseytools mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf)
endif()

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
