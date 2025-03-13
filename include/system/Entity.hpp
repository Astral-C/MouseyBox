#ifndef DISABLE_ENTITY
#ifndef __MB_ENTITY_H__
#define __MB_ENTITY_H__
#include <SDL3/SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <system/json.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/RenderableType.hpp>

namespace mb {

    class Entity {
    private:

        uint8_t mInUse { false };
        uint8_t mCollide { false };
        uint8_t mGrounded { false };
        uint8_t mOnWall { false };
        uint32_t mId { 0xFFFFFFFF };
        uint32_t mTags { 0x00000000 };

        Entity* mNextEntity { nullptr };
        Entity* mPrevEntity { nullptr };

        std::weak_ptr<Graphics::Renderable> mRenderable {};
        std::string mName { "Entity" };

        Entity(){}
        ~Entity(){}

    public:
        int32_t mHealth { 100 };
        uint32_t mState { 0 };
        uint32_t mChannel { 0xFFFFFFFF };
        Entity* mWorld { nullptr };
        Entity* mParent { nullptr };

        float mX { 0 }, mY { 0 };
        float mVelX { 0 }, mVelY { 0 };
        
        std::function<void(Entity*)> mUpdate {};
        std::function<void(Entity*)> mReady {};
        std::function<void(Entity*)> mFree {};

        char mData[128] {0}; // user data buffer

        void SetRenderable(std::shared_ptr<Graphics::Renderable> s) { mRenderable = s; }
        std::shared_ptr<Graphics::Renderable> GetRenderable() { return mRenderable.lock(); }
        SDL_FRect GetRect() { return { mRenderable.lock()->GetRect()->x, mRenderable.lock()->GetRect()->y, (mRenderable.lock()->GetRect()->w * mRenderable.lock()->GetScale()), (mRenderable.lock()->GetRect()->h * mRenderable.lock()->GetScale())}; }
        void SetAnimation(std::string anim) { if(mRenderable.lock()->IsType(RenderableType::Sprite)) dynamic_pointer_cast<Graphics::SpriteInstance>(mRenderable.lock())->SetAnimation(anim); }
        void SetScale(float factor) { mRenderable.lock()->SetScale(factor); }
        float GetScale() { return mRenderable.lock()->GetScale(); }

        void Id(uint32_t t) { mId = t; }
        uint32_t Id() { return mId; }

        void Tag(uint32_t t){ mTags = t; }
        uint32_t Tag() { return mTags; }

        void Grounded(bool g) { mGrounded = g; }
        bool Grounded() { return mGrounded; }


        void OnWall(bool w) { mOnWall = w; }
        bool OnWall() { return mOnWall; }

        bool ToggleCollide() { mCollide = !mCollide; return mCollide; }
        bool ShouldCollide() { return mCollide; }

        static void Initialize(size_t);
        static void Cleanup();

        static Entity* GetById(uint32_t id);
        static Entity* GetNextTagged(Entity* e, int tag);
        
        static Entity* New();
        static void Free(Entity*);

        static void Update();

        static void ForEach(std::function<void(Entity*)>);
        static void ForAll(std::function<void(Entity*)>);
    };

}

#endif
#endif