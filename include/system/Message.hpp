#ifndef __MB_MSG_H__
#define __MB_MSG_H__
#include <cstdint>

namespace mb::Message {

    union Channel {
        uint16_t mData;
    public:
        void SetSlot(uint32_t slot) { if(slot <= 15) mData |= (1 << slot); }
        void ClearSlot(uint32_t slot) { if(slot <= 15) mData &= ~(1 << slot);}
        bool GetSlot(uint32_t slot) { return mData & (1 << slot); }
        Channel(){ mData = 0; }
        ~Channel(){}

    };

    void Initialize(int32_t);
    void Cleanup();

    Channel* GetChannel(uint32_t);

}

#endif