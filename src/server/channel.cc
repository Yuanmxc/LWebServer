#include "channel.h"

#include <sched.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

#include "member.h"
namespace ws {

bool SetCPUaffinity(int param) {
    cpu_set_t mask;
    int a = param;

    CPU_ZERO(&mask);
    CPU_SET(a, &mask);
    // 第一个参数为零的时候默认为调用线程
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {  // 设置线程CPU亲和力
        return false;
    } else {
        return true;
    }
}

void looping(std::promise<std::queue<int>*>& pro, int eventfd, int index) {
    channel rea(eventfd);
    pro.set_value(rea.return_ptr());
    rea._Epoll_.Add(rea, EpollCanRead());
    SetCPUaffinity(index);
    while (true) {
        try {
            EpollEvent_Result Event_Reault(Yuanmxc_Arch::EventResult_Number());

            while (true) {
                rea._Epoll_.Epoll_Wait(Event_Reault);
                for (int i = 0; i < Event_Reault.size(); ++i) {
                    auto& item = Event_Reault[i];
                    int id = item.Return_fd();

                    if (id == eventfd) {
                        uint64_t Temp = 0;
                        read(eventfd, &Temp, sizeof(Temp));
                        while (Temp--) {
                            assert(!rea.return_ptr()->empty());
                            // 从队列中取到的值就是fd，第二个参数是这个fd加入epoll应该触发的事件是什么，默认注册三个事件，加上读事件；
                            rea._Manger_.Opera_Member(
                                std::make_unique<Member>(
                                    rea.return_ptr()->front()),
                                EpollCanRead());
                            rea.return_ptr()->pop();
                        }
                        rea._Epoll_.Modify(rea, EpollCanRead());
                    } else if (item.check(EETRDHUP)) {
                        rea._Manger_.Remove(id);
                    } else if (item.check(EETCOULDREAD)) {
                        rea._Manger_.Reading(id);
                        rea._Manger_.JudgeToClose(id);  // 修改
                    } else if (item.check(EETCOULDWRITE)) {
                        rea._Manger_.Writing(id);
                        rea._Manger_.JudgeToClose(id);
                    }
                }
            }
        } catch (...) {
            std::cerr << "error in : " << std::this_thread::get_id()
                      << std::endl;
        }
    }
    // 此时线程结束,此时我们应该在分发函数中检查每个线程结构是否是有效的；
}

const uint64_t channel_helper::tool = 1;

// 先把工作线程创建好
void channel_helper::loop() {
    for (unsigned int i = 0; i < RealThreadNumber; i++) {
        std::promise<std::queue<int>*> Temp;
        vec.push_back(Temp.get_future());
        int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        pool.push_back(std::thread(looping, std::ref(Temp), fd, i%ThreadNumber));
        pool.back().detach();
        store_.push_back(vec[i].get());
        eventfd_.push_back(fd);
    }
}

// 每到达一个连接就会使用eventfd通信一次
void channel_helper::Distribution(int fd) {
    store_[RoundRobin]->push(fd);
    write(eventfd_[RoundRobin], &channel_helper::tool, sizeof(tool));
    RoundRobin = (RoundRobin + 1) % RealThreadNumber;
}

}  // namespace ws