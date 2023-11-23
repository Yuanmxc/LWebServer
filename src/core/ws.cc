#include "ws.h"

#include <assert.h>
#include <signal.h>
#include <sys/time.h>

#include <iostream>

#include "../base/Yuanmxc_Arch.h"

namespace ws {

int64_t Get_Current_Time() {
    timeval now;
    int ret = gettimeofday(&now, nullptr);
    assert(ret != 1);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

Web_Server::Web_Server()
    : _Epoll_(), _Manger_(_Epoll_), _Server_(Yuanmxc_Arch::MyPort()) {}

void Web_Server::Running() {
    try {
        signal(SIGPIPE, SIG_IGN);
        _Server_.Set_AddrRUseP();
        _Server_.Server_BindAndListen();
        _Epoll_.Add(_Server_, EpollCanRead());
        EpollEvent_Result Event_Reault(Yuanmxc_Arch::EventResult_Number());

        channel_helper Channel_;
        Channel_.loop();

        while (true) {
            _Epoll_.Epoll_Wait(Event_Reault);
            for (int i = 0; i < Event_Reault.size(); ++i) {
                auto& item = Event_Reault[i];
                int id = item.Return_fd();

                if(id == _Server_.fd()){ //这里放入事件循环
                        _Server_.Server_Accept([&](int fd){Channel_.Distribution(fd);});
                        _Epoll_.Modify(_Server_, EpollCanRead());
                    }
            }
        }
    } catch (std::exception& err) {
        std::cout << err.what() << std::endl;
    }
}
}  // namespace ws