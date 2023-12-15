#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/tcp.h>

#include <functional>
#include <memory>

#include "../net/address.h"
#include "../net/socket.h"
#include "../tool/fileopen.h"

namespace ws {
class Server : public Socket {
    using fun = std::function<void(int)>;

   public:
    Server(const Address& addr_)
        : Addr_(std::make_unique<Address>(addr_)), FileOpen() {}
    Server(const char* buffer, int port)
        : Addr_(std::make_unique<Address>(buffer, port)), FileOpen() {}
    explicit Server(int port)
        : Addr_(std::make_unique<Address>(port)), FileOpen() {}

    std::unique_ptr<Socket> Server_Accept();
    void Server_Accept(fun&& f);
    void Server_BindAndListen();

    int Set_AddrRUseA() {
        return Set_Socket(SO_REUSEADDR, SOL_SOCKET);
    }  // 防止服务器重启受阻
    int Set_AddrRUseP() {
        return Set_Socket(SO_REUSEPORT, SOL_SOCKET);
    }  // 用于解决单线程accept的瓶颈与负载不均衡的问题，我的架构用不到这个，但它很有用；
    int Set_KeepAlive() {
        return Set_Socket(SO_KEEPALIVE, SOL_SOCKET);
    }  // TCP保活机制

    int Set_Nodelay() { return Set_Socket(TCP_NODELAY, SOL_TCP); }  // Nagle
    int Set_COPK() {
        return Set_Socket(TCP_CORK, SOL_TCP);
    }  // TCP_CORK可以提升吞吐量，在确认要传输传输大于MSS的数据时使用
    int Set_QuickAck() {
        return Set_Socket(TCP_QUICKACK, SOL_TCP);
    }  // 禁止延迟确认机制，减少时延
    int Base_Setting() {
        return Set_Socket(TCP_NODELAY | TCP_QUICKACK, SOL_TCP);
    }                  // 减少一次系统调用
    int Set_Linger();  // close时直接发送RST报文

   private:
    std::unique_ptr<Address> Addr_;
    fileopen FileOpen;
    int Set_Socket(int event_type, int level) {
        int buffer_ = 0;
        return setsockopt(fd(), level, event_type, &buffer_, sizeof(buffer_));
    }
};
}  // namespace ws

#endif