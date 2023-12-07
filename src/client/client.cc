#include "client.h"

#include <assert.h>
#include <sys/timerfd.h>

#include <iostream>

#include "../base/config.h"
#include "../net/epoll_event.h"
#include "../net/epoll_event_result.h"
#include "../net/socket.h"
#include "../tool/userbuffer.h"

namespace {
int Suit_TimingWheel_oneparameter = 0;
}

namespace ws {

void Client::ResetEventfd(int Delay) {
    TimerWheel_->TW_Add(++Suit_TimingWheel_oneparameter,
                        std::bind(&Connection::Connect, Connection_.get(),
                                  std::placeholders::_1),
                        Delay);
    struct itimerspec newValue;
    memset(&newValue, 0, sizeof newValue);
    struct itimerspec oldValue;
    memset(&oldValue, 0, sizeof oldValue);
    struct timespec DelayTime;
    memset(&DelayTime, 0, sizeof DelayTime);

    DelayTime.tv_sec = static_cast<time_t>(Delay);
    DelayTime.tv_nsec = static_cast<long>(0);
    newValue.it_value = std::move(DelayTime);

    // 第二个参数为零表示相对定时器 TFD_TIMER_ABSTIME为绝对定时器
    int ret = ::timerfd_settime(eventfd_.fd(), 0, &newValue, &oldValue);
    if (ret == -1)
        std::cerr << "Client::ResetEventfd.timerfd_settime failture.\n";
}

void Client::SetFd_inSockers(int fd) {
    std::unique_ptr<Socket> ptr(new Socket(fd));
    Epoll_->Add(*ptr, EpollCanRead());
    Sockers_[fd] = std::move(ptr);
}
// 服务端未开启而客户端进行非阻塞connect时 epoll中会收到EPOLLRDHUP事件
void Client::Remove(int fd) {
    if (Sockers_.find(fd) == Sockers_.end())
        throw std::logic_error("Client::Remove What happend?");
    Epoll_->Remove(*Sockers_[fd], EpollTypeBase());
    Sockers_.erase(fd);
}

void Client::Connect() { Connection_->Connect(0); }

void Client::Run() {
    Epoll_->Add(eventfd_, EpollCanRead());
    EpollEvent_Result Event_Reault(Yuanmxc_Arch::EventResult_Number());

    while (true) {
        Epoll_->Epoll_Wait(Event_Reault);
        for (int i = 0; i < Event_Reault.size(); ++i) {
            auto& item = Event_Reault[i];
            int id = item.Return_fd();

            if (id == eventfd_.fd()) {
                TimerWheel_->TW_Tick();
                Epoll_->Modify(eventfd_, EpollCanRead());
            } else if (item.check(EETRDHUP)) {  // 断开连接
                Remove(id);
            } else if (item.check(EETCOULDREAD)) {  // 可读
                std::shared_ptr<UserBuffer> Buffer_(new UserBuffer(8096));
                Sockers_[id]->Read(Buffer_);
                std::string Content(Buffer_->ReadPtr(), Buffer_->Readable());
                std::cout << Content << std::endl;
            } else if (item.check(EETCOULDWRITE)) {
                Connection_->HandleWrite(
                    id, std::bind(&Client::SetFd_inSockers, this,
                                  std::placeholders::_1));
            }
        }
    }
}

}  // namespace ws