#include <system/Message.hpp>

namespace mb::Message {

    namespace {
        uint32_t mChannelCount;
        Channel* mChannels;  
    };

    void Initialize(int32_t channelCount){
        mChannelCount = channelCount;
        mChannels = new Channel[channelCount];
    }

    void Cleanup(){
        if(mChannels != nullptr){
            delete[] mChannels;
        }
    }

    Channel* GetChannel(uint32_t channel){
        if(channel < mChannelCount){
            return &mChannels[channel];
        } else {
            return nullptr;
        }
    }

}