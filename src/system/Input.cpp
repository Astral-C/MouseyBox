#include <system/Log.hpp>
#include <system/Input.hpp>
#include <system/Math.hpp>
#include <map>

namespace mb::Input {
    namespace {
        int mKeyCount { 0 };
        int mJoystickIndex { 0 };
        SDL_Joystick* mJoysticks[4] { nullptr, nullptr, nullptr, nullptr };
        const bool* mKeyboardState { nullptr };
        bool* mKeyboardStatePrevious { nullptr };
    }

    namespace Mouse {
        uint32_t mPrevState;
        uint32_t mCurrentState;
        mb::Math::Vec2<float> mPosition;

        bool Released(uint32_t mask){
            return (mPrevState & SDL_BUTTON_MASK(mask)) && !(mCurrentState & SDL_BUTTON_MASK(mask));
        }

        bool Pressed(uint32_t mask){
            return !(mPrevState & SDL_BUTTON_MASK(mask)) && (mCurrentState & SDL_BUTTON_MASK(mask));
        }

        mb::Math::Vec2<float> GetPosition() {
            return mPosition;
        }
    };

    void Initialize(){
        mKeyboardState = SDL_GetKeyboardState(&mKeyCount);
        if(mKeyCount != 0){
            mKeyboardStatePrevious = new bool[mKeyCount];
            Mouse::mCurrentState = 0;
            Mouse::mPrevState = 0;
        }
    } 

    void OpenPrimaryJoystick(){
        mJoysticks[0] = SDL_OpenJoystick(0);
    }

    bool OpenJoysticks(int numJoysticks){
        if(numJoysticks > 4){
            return false;
        }
        for(int j = 0; j < numJoysticks; j++){
            mJoysticks[j] = SDL_OpenJoystick(j);
            if(mJoysticks[j] == nullptr){
                Log::Debug(std::format("Error Opening Joystick {}", SDL_GetError()));
                return false;
            }
        }
        return true;
    }

    int16_t JoystickAxis(int axis, int idx){
        if(mJoysticks[idx] == nullptr) return 0;
        return SDL_GetJoystickAxis(mJoysticks[idx], axis);
    }

    bool JoystickButton(int button, int idx){
        if(mJoysticks[idx] == nullptr) return false;
        return SDL_GetJoystickButton(mJoysticks[idx], button);
    }

    void Cleanup(){
        SDL_CloseJoystick(mJoysticks[0]);
        if(mKeyboardStatePrevious != nullptr) delete[] mKeyboardStatePrevious;
    }

    SDL_Joystick* GetJoystick(int idx){
        return mJoysticks[idx];
    }

    //void RegisterCommandsFromFile();

    void Update(){
        if(mKeyboardStatePrevious != nullptr) memcpy(mKeyboardStatePrevious, mKeyboardState, mKeyCount*sizeof(uint8_t));
        SDL_PumpEvents();
        mKeyboardState = SDL_GetKeyboardState(&mKeyCount);
        Mouse::mPrevState = Mouse::mCurrentState;
        Mouse::mCurrentState = SDL_GetMouseState(&Mouse::mPosition.x, &Mouse::mPosition.y);
    }

    bool AnyKey(){
        for(int k = 0; k < mKeyCount; k++){
            if(mKeyboardState[k] == 1) return true;
        }
        return false;
    }

    bool GetKeyPressed(SDL_Keycode key){
        if(SDL_GetScancodeFromKey(key, nullptr) < mKeyCount){
            return !mKeyboardStatePrevious[SDL_GetScancodeFromKey(key, nullptr)] && mKeyboardState[SDL_GetScancodeFromKey(key, nullptr)];
        }
        return false;
    }

    bool GetKeyHeld(SDL_Keycode key){
        if(SDL_GetScancodeFromKey(key, nullptr) < mKeyCount){
            return mKeyboardStatePrevious[SDL_GetScancodeFromKey(key, nullptr)] && mKeyboardState[SDL_GetScancodeFromKey(key, nullptr)];
        }
        return false;
    }

    bool GetKeyReleased(SDL_Keycode key){
        if(SDL_GetScancodeFromKey(key, nullptr) < mKeyCount){
            return mKeyboardStatePrevious[SDL_GetScancodeFromKey(key, nullptr)] && !mKeyboardState[SDL_GetScancodeFromKey(key, nullptr)];
        }
        return false;
    }

    bool GetKey(SDL_Keycode key){
        if(SDL_GetScancodeFromKey(key, nullptr) < mKeyCount) return mKeyboardState[SDL_GetScancodeFromKey(key, nullptr)];
        return false;
    }

}