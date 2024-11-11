#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY

#include <set>
#include <memory>
#include <iostream>
#include <asio.hpp>
#include <system/Containers/ThreadSafeQueue.hpp>

namespace mb {

template<typename T>
class Connection : public std::enable_shared_from_this<Connection<T>> {
private:
    asio::mutable_buffer mBuffer{1024};
    asio::ip::udp::endpoint mEndpoint;
    asio::ip::udp::socket mSocket;

public:
    void Send(T message);
};

template <typename T>
class Server : public std::enable_shared_from_this<Server<T>> {
private:
    asio::io_context mContext;

    std::array<uint8_t, 1024> mBuffer{'\0'};
    asio::ip::udp::endpoint mEndpoint, mSenderEndpoint;
    std::shared_ptr<asio::ip::udp::socket> mSocket;
    
    std::set<Connection<T>> mClients;

    std::thread mServeThread;

    TSQ<T> mMessageQueue;

    void Listen(){
        mSocket->async_receive_from(asio::buffer(mBuffer), mSenderEndpoint, [&](const asio::error_code& error, std::size_t bytes_transferred){
            mb::Log::DebugFrom("MouseyBoxNetworking", "Got Connection Request from {}", mSenderEndpoint.address().to_string());
            Listen();
        });
    }

public:

    void Broadcast(){}
    
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