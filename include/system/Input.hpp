#ifndef __MB_INPUT_H__
#define __MB_INPUT_H__
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <system/Math.hpp>

namespace mb::Input {

    namespace Mouse {
        bool Released(uint32_t mask);
        bool Pressed(uint32_t mask);
        mb::Math::Vec2<int> GetPosition();
    };

    enum CommandType {
        AXIS,
        BUTTON
    };

    struct Command {
        CommandType type;
        uint32_t value;
        SDL_KeyCode key;
        SDL_GameControllerAxis axis;
        SDL_GameControllerButton button;
    };

    SDL_Joystick* GetJoystick(int idx=0);

    void Initialize();
    void Cleanup();
    void Update();
    bool GetKey(SDL_KeyCode);
    bool GetKeyPressed(SDL_KeyCode);
    bool GetKeyHeld(SDL_KeyCode);
    bool GetKeyReleased(SDL_KeyCode);
    
    void OpenPrimaryJoystick();
    bool OpenJoysticks(int);

    int16_t JoystickAxis(int, int idx=0);
    bool JoystickButton(int, int idx=0);

    void RegisterCommand(std::string);
    void SetCommandKey(std::string, SDL_KeyCode);
    void SetCommandButton(std::string, SDL_GameControllerButton);
    void SetCommandAxis(std::string SDL_GameControllerAxis);

    int32_t GetValue();

}

#endif