#ifndef MANGER_H_
#define MANGER_H_

#include <memory>
#include <unordered_map>

#include "../base/copyable.h"
#include "../net/epoll.h"
#include "../net/epoll_event.h"
#include "../net/socket.h"
#include "../tool/timing_wheel.h"
#include "member.h"

namespace ws {
class Manger : public Copyable {
    using Fun = std::function<int(int)>;

   public:
    explicit Manger(Epoll& _epoll)
        : _Epoll_(_epoll), Timer_Wheel_(std::make_unique<TimerWheel>()) {}

    int Opera_Member(std::unique_ptr<Member>&, EpollEventType&);
    int Opera_Member(std::unique_ptr<Member>&&, EpollEventType&&);
    int Opera_Member(std::unique_ptr<Member>&, EpollEventType&&);
    int Opera_Member(std::unique_ptr<Member>&&, EpollEventType&);

    void TimeWheel(int fd);

    void Reading(int fd, long time = -1);
    void Writing(int fd, long time = -1);
    int JudgeToClose(int fd);
    int Update(int fd);
    int UpdateWrite(int fd);

    int Remove(int fd);
    bool Exist(int fd) const& {
        if (Fd_To_Member.find(fd) != Fd_To_Member.end()) return true;
        return false;
    }

   private:
    std::unique_ptr<TimerWheel> Timer_Wheel_;
    Epoll& _Epoll_;
    std::unordered_map<int, std::unique_ptr<Member>> Fd_To_Member;
};
}  // namespace ws

#endif