cmake_minimum_required(VERSION 3.10)
project(MouseyBox)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)


option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(ENABLE_OPENMPT "Enable OpenMPT in Audio" OFF)
option(DISABLE_ENTITY "Disalbe Built in Entity System" OFF)

if(ENABLE_OPENMPT)
    add_compile_definitions(ENABLE_OPENMPT)
endif()

if(DISABLE_ENTITY)
    add_compile_definitions(DISABLE_ENTITY)
endif()

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
add_subdirectory(lib/libxm/src libxm_build)

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

if(ENABLE_OPENMPT)
    add_compile_definitions(ENABLE_OPENMPT)
    target_link_libraries(mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf openmpt xm)
else()
     target_link_libraries(mouseybox SDL3::SDL3 SDL3_ttf::SDL3_ttf xm)
endif()

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
