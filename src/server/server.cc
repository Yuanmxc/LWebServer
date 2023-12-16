#include "server.h"

#include <assert.h>
#include <errno.h>

#include <iostream>

namespace {
template <typename T>
T max(T&& a, T&& b) {
    return a > b ? a : b;
}
}  // namespace

namespace ws {
std::unique_ptr<Socket> Server::Server_Accept() {
    return std::make_unique<Socket>(
        ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK));
}

void Server::Server_Accept(fun&& f) {
    // 套接字被设置成非阻塞，需要一个while循环来接收
    while (1) {
        int ret = 0;
        ret = ::accept4(fd(), nullptr, nullptr,
                        SOCK_NONBLOCK);  // 减少一次系统调用
        if (ret != -1) {
            // 成功以后才会分发套接字；失败直接退出就可以了；
            f(ret);
        } else if (ret == -1 && errno == EMFILE) {
            // 只有一个线程accept，所以此做法可以保证安全；
            fileopen_helper prevent(FileOpen);
            // 防止堆积在全连接队列中，直接告诉对端关闭连接；
            ret = ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK);
            ::close(fd());
            break;
        } else if (ret == -1 && errno == EWOULDBLOCK) {  // EWOULDBLOCK
            continue;
        } else if (
            ret ==
            -1) {  // ECONNABORTED
                   // 在等待队列的时候被关闭了，不是非阻塞套及字的话会一直阻塞;
            break;
        } else {
            std::cerr
                << "ERROR : Server::Server_Accept, unexpected situation.\n";
        }
    }
}

void Server::Server_BindAndListen() {
    assert(Addr_ != nullptr);
    int para1 = bind(fd(), Addr_->Return_Pointer(), Addr_->Return_length());
    if (para1 == -1)
        throw std::runtime_error("'Server_BindAndListen' : error in bind.");
    int para2 = listen(fd(), ::max(SOMAXCONN, 1024));
    if (para2 == -1)
        throw std::runtime_error("'Server_BindAndListen' : error in listen.");
}

inline int Server::Set_Linger() {
    struct linger buffer_ = {1, 0};
    return setsockopt(fd(), SOL_SOCKET, SO_LINGER, &buffer_, sizeof(buffer_));
}
}  // namespace ws