#ifndef __MB_INPUT_H__
#define __MB_INPUT_H__
#include <SDL3/SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <system/Math.hpp>

namespace mb::Input {

    namespace Mouse {
        bool Released(uint32_t mask);
        bool Pressed(uint32_t mask);
        mb::Math::Vec2<float> GetPosition();
    };

    enum CommandType {
        AXIS,
        BUTTON
    };

    struct Command {
        CommandType type;
        uint32_t value;
        SDL_Keycode key;
        SDL_GamepadAxis axis;
        SDL_GamepadButton button;
    };

    SDL_Joystick* GetJoystick(int idx=0);

    void Initialize();
    void Cleanup();
    void Update();
    bool GetKey(SDL_Keycode);
    bool GetKeyPressed(SDL_Keycode);
    bool GetKeyHeld(SDL_Keycode);
    bool GetKeyReleased(SDL_Keycode);
    bool AnyKey();
    
    void OpenPrimaryJoystick();
    bool OpenJoysticks(int);

    int16_t JoystickAxis(int, int idx=0);
    bool JoystickButton(int, int idx=0);

    void RegisterCommand(std::string);
    void SetCommandKey(std::string, SDL_Keycode);
    void SetCommandButton(std::string, SDL_GamepadButton);
    void SetCommandAxis(std::string SDL_GamepadAxis);

    int32_t GetValue();

}

#endif