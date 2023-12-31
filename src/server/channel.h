#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <assert.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <future>
#include <queue>
#include <thread>
#include <vector>

#include "../base/config.h"
#include "../base/havefd.h"
#include "../base/nocopy.h"
#include "../net/epoll.h"
#include "../tool/ThreadSafeQueue/lockfreequeue.h"
#include "../tool/loadbalance.h"
#include "manger.h"

namespace ws {
extern template class LockFreeQueue<ThreadLoadData>;
// 每个工作线程的类
class channel : public Nocopy, public Havefd {
   private:
    std::queue<int> ptr_que;
    Manger _Manger_;  // 负责管理每一个线程连接
    Epoll _Epoll_;
    int eventfd;

   public:
    explicit channel(int fd) : eventfd(fd), _Epoll_(), _Manger_(_Epoll_) {}

    int fd() const& noexcept override { return eventfd; }

    std::queue<int>* return_ptr() noexcept { return &ptr_que; }

    friend void looping(std::promise<std::queue<int>*>& pro, int eventfd,
                        int index, LockFreeQueue<ThreadLoadData>& que);
};

// channel_helper负责分发文件描述符
class channel_helper : public Nocopy {
   private:
    std::vector<std::thread> pool;
    std::vector<std::future<std::queue<int>*> > vec;
    std::vector<std::queue<int>*> store_;
    std::vector<int> eventfd_;
    LoadBalance& TrueLD;
    int RoundRobinValue =
        0;  // 可以根据每个线程的实际吞吐量去做一个负载均衡，不过这样需要一个线程安全的数据结构去做负载均衡；
    static const uint64_t tool;
    const int ThreadNumber = std::thread::hardware_concurrency();
    const unsigned int RealThreadNumber = std::max<int>(1, ThreadNumber - 1);

   public:
    explicit channel_helper(LoadBalance& LD) : TrueLD(LD) {}

    void loop();

    void Distribution(int fd);
    // 几种负载均衡的算法
    int RoundRobin() & noexcept;
    int WeightedRoundRobin() &;
};

}  // namespace ws

#endif  // CHANNEL_H_