#ifndef __MB_SERVER_H__
#define __MB_SERVER_H__
#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY

#include <set>
#include <map>
#include <memory>
#include <iostream>
#include <asio.hpp>
#include <system/Network/Packet.hpp>
#include <system/Containers/ThreadSafeQueue.hpp>

namespace mb {

template<typename T>
class Connection : public std::enable_shared_from_this<Connection<T>> {
private:
    asio::ip::udp::endpoint mEndpoint;

public:
    asio::ip::udp::endpoint& GetEndpoint() { return mEndpoint; }

    Connection(asio::io_context& ctx, asio::ip::udp::endpoint endpoint) : mEndpoint(endpoint) {}
    ~Connection(){}

};

template <typename T>
class Server : public std::enable_shared_from_this<Server<T>> {
private:
    std::size_t mClientTimeout { 500 };
    asio::io_context mContext;

    asio::ip::udp::endpoint mEndpoint;
    std::shared_ptr<asio::ip::udp::socket> mSocket;
    
    std::map<std::string, std::shared_ptr<Connection<T>>> mClients;

    std::thread mServeThread;

    TSQ<Packet<T>> mMessageQueue;

    void Listen(){
        mSocket->async_wait(asio::socket_base::wait_type::wait_read, [&](const asio::error_code& error){
            //mb::Log::DebugFrom("MouseyBoxNetworking", "Got Connection Request from {}", mSenderEndpoint.address().to_string());

            if(mSocket->available() >= 8){
                uint8_t packetData[mSocket->available()];
                uint32_t packetSize = mSocket->available();

                asio::ip::udp::endpoint mSenderEndpoint;
                mSocket->receive_from(asio::buffer(packetData, mSocket->available()), mSenderEndpoint);
                
                if(!mClients.contains(mSenderEndpoint.address().to_string())){
                    mClients[mSenderEndpoint.address().to_string()] = std::make_shared<Connection<T>>(mContext, mSenderEndpoint);
                } else {
                    // set timeout
                    //mClients[mSenderEndpoint.address().to_string()].timeout = 
                }

                bStream::CMemoryStream packetStream(packetData, packetSize, bStream::Endianess::Little, bStream::OpenMode::In);

                Packet<T> packet = {};
                packet.mType = static_cast<T>(packetStream.readUInt32());
                packet.mSize = packetStream.readUInt32();

                if(packet.mSize > 0){
                    packet.mData.resize(packet.mSize);
                    packetStream.readBytesTo(packet.mData.data(), packet.mSize);
                }

                packet.mSender = mSenderEndpoint;

                mMessageQueue.push(packet);

            }

            Listen();
        });
    }


public:

    bool HasMessages(){
        return !mMessageQueue.empty();
    }

    Packet<T> ConsumeMessage(){
        Packet<T> p;
        mMessageQueue.pop(p);
        return p;
    }

    void Send(Packet<T>& msg){
        uint8_t packetData[sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize];
        std::memset(packetData, 0, sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize);

        std::memcpy(packetData, reinterpret_cast<uint8_t*>(&msg), sizeof(msg.mType) + sizeof(msg.mSize));
        std::memcpy(packetData + sizeof(msg.mType) + sizeof(msg.mSize), msg.mData.data(), msg.mSize);

        mSocket->send_to(asio::buffer(packetData, sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize), msg.mSender);
    }

    void BroadcastMessage(Packet<T>& msg, std::string filter){
        for(auto [addr, client] : mClients){
            mb::Log::Debug("Check & Broadcast: {} {}", addr, filter);    
            if(addr != filter){
                uint8_t packetData[sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize];

                std::memcpy(packetData, reinterpret_cast<uint8_t*>(&msg), sizeof(msg.mType) + sizeof(msg.mSize));
                std::memcpy(packetData + sizeof(msg.mType) + sizeof(msg.mSize), msg.mData.data(), msg.mSize);
                
                mSocket->send_to(asio::buffer(packetData, sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize), client->GetEndpoint());
            }
        }
    }

    void Start(){
        mb::Log::Debug("Starting Server ASIO Context...");
        mServeThread = std::thread([&](){ mContext.run(); });
    }

    void Stop(){
        mb::Log::Debug("Stopping Server...");
        mContext.stop();
        if(mServeThread.joinable()) mServeThread.join();
    }

    Server(int port, std::string addr="0.0.0.0") : mEndpoint(asio::ip::make_address(addr), port),  mSocket(std::make_shared<asio::ip::udp::socket>(mContext, mEndpoint)){
        Listen();
    }

    ~Server(){
        Stop();
    }
};

}

#endif