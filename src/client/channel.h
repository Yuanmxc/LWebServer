#ifndef CLIENT_CHANNEL_H_
#define CLIENT_CHANNEL_H_

#include <sys/eventfd.h>

#include <functional>
#include <memory>
#include <queue>

#include "../base/nocopy.h"
#include "../net/epoll.h"
#include "../net/epoll_event.h"
#include "eventfdWrapper.h"

namespace ws {

// 用于向循环线程传递数据
class ClientChannel : public Nocopy {
    using EventType = std::function<void()>;

   private:
    std::queue<EventType> Events;
    std::shared_ptr<Epoll> ClientEpoll;
    EventFdWrapper Eventfd;
    const uint64_t padding = 1;

   public:
    explicit ClientChannel(std::shared_ptr<Epoll> ptr)
        : ClientEpoll(ptr), Eventfd(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) {
        ptr->Add(Eventfd, EpollCanRead());
    }

    void push(const std::function<void()>& fun) {
        ::write(Eventfd.fd(), &padding, sizeof padding);
        Events.push(std::move(fun));
    }

    void RunAndPop() {
        uint64_t Temp = 0;
        ::read(Eventfd.fd(), &Temp, sizeof(Temp));
        while (Temp--) {
            std::function<void()> fun = std::move(Events.front());
            Events.pop();
            fun();
        }
    }
    int fd() const noexcept { return Eventfd.fd(); }
};

}  // namespace ws

#endif  // CLIENT_CHANNEL_H_