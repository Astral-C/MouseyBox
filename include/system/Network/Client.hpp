#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY

#include <asio.hpp>
#include <system/Containers/ThreadSafeQueue.hpp>


namespace mb {
template <typename T>
class Client : public std::enable_shared_from_this<Client<T>> {
private:
    asio::io_context mContext;

    asio::ip::udp::endpoint mEndpoint;
    asio::ip::udp::endpoint mServerEndpoint;
    std::shared_ptr<asio::ip::udp::socket> mSocket;
    
    std::thread mClientThread;

    TSQ<Packet<T>> mMessageQueue;

    void Listen(){
        mSocket->async_wait(asio::socket_base::wait_type::wait_read, [&](const asio::error_code& error){
            if(mSocket->available() >= 8){
                uint8_t packetData[mSocket->available()];
                uint32_t packetSize = mSocket->available();

                mSocket->receive_from(asio::buffer(packetData, mSocket->available()), mServerEndpoint);
                
                bStream::CMemoryStream packetStream(packetData, packetSize, bStream::Endianess::Little, bStream::OpenMode::In);

                Packet<T> packet = {};
                packet.mType = static_cast<T>(packetStream.readUInt32());
                packet.mSize = packetStream.readUInt32();

                if(packet.mSize > 0){
                    packet.mData.resize(packet.mSize);
                    packetStream.readBytesTo(packet.mData.data(), packet.mSize);
                }

                packet.mSender = mServerEndpoint;

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

    asio::ip::udp::endpoint ClientEndpoint() { return mEndpoint; }

    void Send(Packet<T>& msg){
        uint8_t packetData[sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize];
        std::memset(packetData, 0, sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize);

        std::memcpy(packetData, reinterpret_cast<uint8_t*>(&msg), sizeof(msg.mType) + sizeof(msg.mSize));
        std::memcpy(packetData + sizeof(msg.mType) + sizeof(msg.mSize), msg.mData.data(), msg.mSize);

        mSocket->send_to(asio::buffer(packetData, sizeof(msg.mType) + sizeof(msg.mSize) + msg.mSize), mServerEndpoint);
    }

    void Start(){
        mb::Log::Debug("Starting Client ASIO Context...");
        mClientThread = std::thread([&](){ mContext.run(); });
    }

    void Stop(){
        mb::Log::Debug("Stopping Client...");
        mContext.stop();
        if(mClientThread.joinable()) mClientThread.join();
    }

    void _UpdateLocal(std::string localaddr, int port){
        mEndpoint = asio::ip::udp::endpoint(asio::ip::make_address(localaddr), port);
        mSocket = std::make_shared<asio::ip::udp::socket>(mContext, mEndpoint);
    }

    Client(std::string addr, std::string localaddr, int port) : mEndpoint(asio::ip::udp::endpoint(asio::ip::make_address(localaddr), port)), mServerEndpoint(asio::ip::udp::endpoint(asio::ip::make_address(addr), port)), mSocket(std::make_shared<asio::ip::udp::socket>(mContext, mEndpoint)){
        Listen();
    }

    Client(std::string addr, int port) : mServerEndpoint(asio::ip::udp::endpoint(asio::ip::make_address(addr), port)), mSocket(std::make_shared<asio::ip::udp::socket>(mContext, asio::ip::udp::endpoint(asio::ip::make_address("0.0.0.0"), port))){
        Listen();
    }

    ~Client(){
        // send a disconnect packet
        Stop();
    }
};

}