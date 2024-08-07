#include <Game.hpp>
#include <system/Application.hpp>
#include <system/Entity.hpp>
#include <system/Input.hpp>
#include <iostream>

int main(int argc, char *argv[]){
    mb::Log::InfoFrom("MouseyBox", "Main Entrypoint Started");
    if(MouseyBoxExample::GetGame()->Initialize()){
        MouseyBoxExample::GetGame()->Run();
    }
    return 0;
}
