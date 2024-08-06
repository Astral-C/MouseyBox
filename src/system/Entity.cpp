#include <system/Entity.hpp>
#include <system/Log.hpp>

namespace mb {
    namespace {
        uint8_t mInitialized { 0 };
        size_t mEntityCount { 0 };
        Entity*  mEntities { nullptr };

        Entity* mFreeHead { nullptr };
        Entity* mUsedHead { nullptr };
        
    }

    void Entity::Cleanup(){
        if(mEntities != nullptr){
            delete[] mEntities;
        }
        mEntities = nullptr;
    }

    void Entity::Initialize(size_t maxEntities){
        if(mEntities != nullptr) return;
        mEntityCount = maxEntities;
        mEntities = new Entity[maxEntities];
        mFreeHead = mEntities;

        Entity* prev = nullptr;
        for(size_t t = 0; t < mEntityCount; t++){
            Entity* e = &mEntities[t];
            e->mId = t;
            
            e->mNextEntity = &mEntities[t + 1];
            e->mPrevEntity = prev;
            prev = e;
        }

        mEntities[mEntityCount - 1].mNextEntity = nullptr;

    }

    Entity* Entity::New(){
        Entity* e = mFreeHead;
        if(mFreeHead == nullptr) return nullptr;
        e->mInUse = 1;
        
        mFreeHead = mFreeHead->mNextEntity;
        
        if(mUsedHead == nullptr){
            // Used-list is empty, init used-list with this entity as the current head
            mUsedHead = e;
            e->mPrevEntity = nullptr;
            e->mNextEntity = nullptr;
        } else {
            // Used-list is not empty, push to front of current used list
            mUsedHead->mPrevEntity = e;
            e->mNextEntity = mUsedHead;
            e->mPrevEntity = nullptr;
            mUsedHead = e;
        }

        //e->mCreate();

        return e;
    }

    /*
    Entity* Entity::Get(std::function<bool(Entity*)> comp){
        for(Entity* e = &mEntities[0]; e < mEntities + mEntityCount; e++){
            if(e->mId){
                return 
            }
        }
        return nullptr;
    }
    */

    Entity* Entity::GetById(uint32_t id){
        for(Entity* e = mUsedHead; e != nullptr; e = e->mNextEntity){
            if(e->mId == id){
                return e;
            }
        }
        return nullptr;
    }

    Entity* Entity::GetNextTagged(Entity* e, int tag){
        Entity* entity = e;

        if(entity == nullptr){
            entity = mUsedHead;
        } else {
            entity = entity->mNextEntity;
        }

        for(; entity != nullptr; entity = entity->mNextEntity){
            if(entity->mTags == (uint32_t)tag){
                return entity;
            }
        }
        return nullptr;
    }

    void Entity::Free(Entity* e){
        // Not one of the entities we manage. Bye girl!
        if(e < mEntities && e > mEntities + mEntityCount) return;

        // Remove from used-list
        if(e->mPrevEntity != nullptr){
            e->mPrevEntity->mNextEntity = e->mNextEntity;
        }

        if(e->mNextEntity != nullptr){
            e->mNextEntity->mPrevEntity = e->mPrevEntity;
        }

        if(e == mUsedHead){
            mUsedHead = e->mNextEntity;
        }

        if(e->mFree){
            e->mFree(e);
        }

        // Clear entity
        e->mInUse = 0;
        e->mX = 0;
        e->mY = 0;
        memset(e->mData, 0, sizeof(e->mData));
        e->mChannel = 0xFFFFFFFF;
        e->mCollide = 0;
        e->mGrounded = 0;
        e->mId = 0xFFFFFFFF;
        e->mWorld = nullptr;
        e->mName = "";
        e->mTags = 0xFFFFFFFF;
        e->mState = 0;
        
        // Add to front of free list
        e->mPrevEntity = nullptr;
        e->mNextEntity = mFreeHead;

        mFreeHead->mPrevEntity = e;
        mFreeHead = e;
    }

    void Entity::Update(){
        Entity* entity = mUsedHead;
        while(entity != nullptr){
            Entity* nextEnt = entity->mNextEntity; // keep track of the next in use ent in the event that this one is freed in its update

            if(entity->mRenderable.lock()){
                SDL_Rect* r = entity->mRenderable.lock()->GetRect();
                r->x = entity->mX;
                r->y = entity->mY;
            }

            if(entity->mUpdate){
                entity->mUpdate(entity);
            }

            entity = nextEnt;
        }
    }

    void Entity::ForEach(std::function<void(Entity*)> func){
        Entity* entity = mUsedHead;
        while(entity != nullptr){
            Entity* nextEnt = entity->mNextEntity; // keep track of the next in use ent in the event that this one is freed in its update

            func(entity);

            entity = nextEnt;
        }
    }

}