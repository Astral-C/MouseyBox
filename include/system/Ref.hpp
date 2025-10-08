#ifndef __MB_REF_H__
#define __MB_REF_H__
#include <system/Log.hpp>

namespace mb {
    template<typename T>
    class Ref {

        template<typename U>
        struct Block {
            uint32_t mRefCount { 0 };
            U* mPtr { nullptr };
            Block(U* ptr){
                mRefCount = 1;
                mPtr = ptr;
            }
            ~Block(){
                if(mPtr != nullptr){
                    delete mPtr;
                }
            }
            
        };

        Block<T>* mBlock { nullptr };
        
    public:

        Ref<T> operator=(Ref<T> o){
            mBlock = o.mBlock;
            if(mBlock != nullptr){
                mBlock->mRefCount++;
            }
            return *this;
        }

        Ref<T> operator=(Ref<T>& o){
            mBlock = o.mBlock;
            if(mBlock != nullptr){
                mBlock->mRefCount++;
            }
            return *this;
        }

        Ref<T>& operator=(Ref<T>&& o){
            std::memmove(this, o);
        }

        template <typename... Args>
        static Ref<T> New(Args&&... args){
            Ref<T> ref;
            ref.mBlock = new Block<T>(new T(args...));
            return ref;
        }

        ~Ref(){
            if(mBlock != nullptr){
                mBlock->mRefCount--;
                if(mBlock->mRefCount == 0){
                    mb::Log::DebugFrom("MouseyRef", "No more refs, deleting");
                    delete mBlock;
                }
            }
        }
    };
}

#endif