#include <system/Log.hpp>
#include <system/Input.hpp>
#include <system/Math.hpp>
#include <map>

namespace mb::Input {
    namespace {
        int mKeyCount { 0 };
        int mJoystickIndex { 0 };
        SDL_Joystick* mJoysticks[4] { nullptr, nullptr, nullptr, nullptr };
        const uint8_t* mKeyboardState { nullptr };
        uint8_t* mKeyboardStatePrevious { nullptr };
    }

    namespace Mouse {
        uint32_t mPrevState;
        uint32_t mCurrentState;
        mb::Math::Vec2<int> mPosition;

        bool Released(uint32_t mask){
            return (mPrevState & SDL_BUTTON(mask)) && !(mCurrentState & SDL_BUTTON(mask));
        }

        bool Pressed(uint32_t mask){
            return !(mPrevState & SDL_BUTTON(mask)) && (mCurrentState & SDL_BUTTON(mask));
        }

        mb::Math::Vec2<int> GetPosition() {
            return mPosition;
        }
    };

    void Initialize(){
        mKeyboardState = static_cast<const uint8_t*>(SDL_GetKeyboardState(&mKeyCount));
        mKeyboardStatePrevious = new uint8_t[mKeyCount];
        Mouse::mCurrentState = 0;
        Mouse::mPrevState = 0;
    } 

    void OpenPrimaryJoystick(){
        mJoysticks[0] = SDL_JoystickOpen(0);
    }

    bool OpenJoysticks(int numJoysticks){
        if(numJoysticks > 4){
            return false;
        }
        for(int j = 0; j < numJoysticks; j++){
            mJoysticks[j] = SDL_JoystickOpen(j);
            if(mJoysticks[j] == nullptr){
                Log::Debug(std::format("Error Opening Joystick {}", SDL_GetError()));
                return false;
            }
        }
        return true;
    }

    int16_t JoystickAxis(int axis, int idx){
        if(mJoysticks[idx] == nullptr) return 0;
        return SDL_JoystickGetAxis(mJoysticks[idx], axis);
    }

    bool JoystickButton(int button, int idx){
        if(mJoysticks[idx] == nullptr) return false;
        return SDL_JoystickGetButton(mJoysticks[idx], button);
    }

    void Cleanup(){
        SDL_JoystickClose(mJoysticks[0]);
        if(mKeyboardStatePrevious != nullptr) delete[] mKeyboardStatePrevious;
    }

    SDL_Joystick* GetJoystick(int idx){
        return mJoysticks[idx];
    }

    void RegisterCommand(std::string){
    }

    void SetCommandKey(std::string, SDL_KeyCode){

    }
    
    void SetCommandButton(std::string, SDL_GameControllerButton){

    }
    
    void SetCommandAxis(std::string SDL_GameControllerAxis){

    }

    void Update(){
        if(mKeyboardStatePrevious != nullptr) memcpy(mKeyboardStatePrevious, mKeyboardState, mKeyCount*sizeof(uint8_t));
        SDL_PumpEvents();
        mKeyboardState = static_cast<const uint8_t*>(SDL_GetKeyboardState(&mKeyCount));
        Mouse::mPrevState = Mouse::mCurrentState;
        Mouse::mCurrentState = SDL_GetMouseState(&Mouse::mPosition.x, &Mouse::mPosition.y);
    }

    bool GetKeyPressed(SDL_KeyCode key){
        if(SDL_GetScancodeFromKey(key) < mKeyCount){
            return !mKeyboardStatePrevious[SDL_GetScancodeFromKey(key)] && mKeyboardState[SDL_GetScancodeFromKey(key)];
        }
        return false;
    }

    bool GetKeyHeld(SDL_KeyCode key){
        if(SDL_GetScancodeFromKey(key) < mKeyCount){
            return mKeyboardStatePrevious[SDL_GetScancodeFromKey(key)] && mKeyboardState[SDL_GetScancodeFromKey(key)];
        }
        return false;
    }

    bool GetKeyReleased(SDL_KeyCode key){
        if(SDL_GetScancodeFromKey(key) < mKeyCount){
            return mKeyboardStatePrevious[SDL_GetScancodeFromKey(key)] && !mKeyboardState[SDL_GetScancodeFromKey(key)];
        }
        return false;
    }

    bool GetKey(SDL_KeyCode key){
        if(SDL_GetScancodeFromKey(key) < mKeyCount) return mKeyboardState[SDL_GetScancodeFromKey(key)];
        return false;
    }

}