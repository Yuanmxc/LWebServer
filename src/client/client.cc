#include "client.h"

#include <assert.h>
#include <sys/timerfd.h>

#include "../base/config.h"
#include "../net/epoll_event.h"
#include "../net/epoll_event_result.h"
#include "../net/socket.h"
#include "../tool/userbuffer.h"

namespace ws {

void Client::ResetEventfd(int Delay) {
    TimerWheel_->TW_Add(eventfd_.fd(),
                        std::bind(&Connection::Connect, Connection_.get(),
                                  std::placeholders::_1),
                        Delay);
    struct itimerspec newValue;
    memset(&newValue, 0, sizeof newValue);
    struct timespec DelayTime;
    memset(&DelayTime, 0, sizeof DelayTime);

    DelayTime.tv_sec = Delay;
    DelayTime.tv_nsec = 0;
    newValue.it_value = std::move(DelayTime);

    int ret = ::timerfd_settime(eventfd_.fd(), 0, &newValue, 0);
    assert(!ret);
}

void Client::SetFd_inSockers(int fd) {
    std::unique_ptr<Socket> ptr(new Socket(fd));
    Epoll_->Add(*ptr, EpollCanRead());
    Sockers_[fd] = std::move(ptr);
}

void Client::Remove(int fd) {
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
                eventfd_.Read();
            } else if (item.check(EETRDHUP)) {  // 断开连接
                Remove(id);
            } else if (item.check(EETCOULDREAD)) {  // 可读
                std::shared_ptr<UserBuffer> Buffer_(new UserBuffer(8096));
                Sockers_[id]->Read(Buffer_);
                std::string Content(Buffer_->ReadPtr(), Buffer_->Readable());
                std::cout << Content << std::endl;
            } else if (item.check(EETCOULDWRITE)) {
                Connection_->HandleWrite(
                    id, std::bind(&ResetEventfd, this, std::placeholders::_1));
            }
        }
    }
}

}  // namespace ws