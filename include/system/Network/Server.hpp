#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY

#include <set>
#include <map>
#include <memory>
#include <iostream>
#include <asio.hpp>
#include <system/Containers/ThreadSafeQueue.hpp>

namespace mb {

template<typename T>
class Connection : public std::enable_shared_from_this<Connection<T>> {
private:
    asio::ip::udp::endpoint& mEndpoint;

public:
    void Send(std::shared_ptr<asio::ip::udp::socket> socket){//T message){
        mb::Log::DebugFrom("MouseyBoxNetworking", "Sending response...");
        socket->send_to(asio::buffer("Response!"), mEndpoint);
    }

    Connection(asio::io_context& ctx, asio::ip::udp::endpoint& endpoint) : mEndpoint(endpoint) {}
    ~Connection(){}

};

template <typename T>
class Server : public std::enable_shared_from_this<Server<T>> {
private:
    asio::io_context mContext;

    std::array<uint8_t, 1024> mBuffer{'\0'};
    asio::ip::udp::endpoint mEndpoint, mSenderEndpoint;
    std::shared_ptr<asio::ip::udp::socket> mSocket;
    
    std::map<std::string, std::shared_ptr<Connection<T>>> mClients;

    std::thread mServeThread;

    TSQ<T> mMessageQueue;

    void Listen(){
        mSocket->async_receive_from(asio::buffer(mBuffer), mSenderEndpoint, [&](const asio::error_code& error, std::size_t bytes_transferred){
            mb::Log::DebugFrom("MouseyBoxNetworking", "Got Connection Request from {}", mSenderEndpoint.address().to_string());

            if(!mClients.contains(mSenderEndpoint.address().to_string())){
                mClients[mSenderEndpoint.address().to_string()] = std::make_shared<Connection<T>>(mContext, mSenderEndpoint);
            } else {
                mClients[mSenderEndpoint.address().to_string()]->Send(mSocket);
            }

            Listen();
        });
    }


public:

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