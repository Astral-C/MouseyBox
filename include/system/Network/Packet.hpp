#ifndef __MB_PACKET_H__
#define __MB_PACKET_H__
#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY
#include <asio.hpp>
#include <cstdint>
#include <vector>


namespace mb {

template<typename>
class Server;

template<typename>
class Client;

template<typename T>
class Packet {
    friend class Server<T>;
    friend class Client<T>;

    T mType { 0 };
    uint32_t mSize { 0 };
    std::vector<uint8_t> mData {};
    asio::ip::udp::endpoint mSender {}; // This gets filled in by client/server code and isnt read from packet data!

    Packet(){}

public:
    bool FromSource(asio::ip::udp::endpoint src) { return src == mSender; }
    asio::ip::udp::endpoint GetSender() { return mSender; }
    T GetType() { return mType; }

    std::vector<uint8_t>& GetData() { return mData; }
    
    Packet(T type, std::size_t size, asio::ip::udp::endpoint target){
        mType = type;
        mSize = size;
        mData.resize(mSize);
        mSender = target;
    }

    ~Packet(){}
};

}

#endif