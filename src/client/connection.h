#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <functional>

#include "../base/config.h"
#include "../base/nocopy.h"
#include "../net/address.h"
#include "../net/epoll.h"
#include "../net/socket.h"

namespace ws {

class Connection : public Nocopy {
   private:
    enum ConnectionState { kDisconnected, kConnecting, kConnected };

    int retryDelayMs_;
    Address ServerAddress;
    ConnectionState states;
    std::shared_ptr<Epoll> ClientEpoll;
    Socket socket_;
    std::function<void(int)> RetryCallBack_;

    static const int kMaxRetryDelayMs = 48;
    static const int KInitRetryDelayMs = 1;

    void SetConnectionState(ConnectionState state) { states = state; }
    void Connecting(const Socket& socket);
    void retry(int fd);

    int getSocketError(int sockfd);
    bool isSelfConnect(int sockfd);
    struct sockaddr_in6 getLocalAddr(int sockfd);
    struct sockaddr_in6 getPeerAddr(int sockfd);

   public:
    explicit Connection(std::shared_ptr<Epoll> ptr)
        : retryDelayMs_(KInitRetryDelayMs),
          states(kDisconnected),
          ClientEpoll(ptr),
          socket_(-1),
          ServerAddress(Yuanmxc_Arch::MyIP(), Yuanmxc_Arch::MyPort()) {}

    int Connect(int padding);
    void HandleWrite(int fd,
                     const std::function<void(int)>& newConnectionCallback);
    void SetTetryCallBack_(const std::function<void(int)>& callback);
};

}  // namespace ws

#endif  // CONNECTION_H_