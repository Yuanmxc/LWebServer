#ifndef LOADBALANCE_H_
#define LOADBALANCE_H_

#include <thread>
#include <vector>

#include "../base/nocopy.h"
#include "ThreadSafeQueue/lockfreequeue.h"

#ifndef __GNUC__

#define __attribute__(x) /*NOTHING*/

#endif

namespace ws {

struct ThreadLoadData {
    uint32_t Throughput;
    uint16_t ThreadId;  // 所属线程下标，accept线程分配；
    uint16_t
        ConnectionNumber;  // 该线程目前存在的长连接数（长连接很难做负载均衡）

    ThreadLoadData(uint32_t thoughtout, uint16_t id, uint16_t connection = 0)
        : Throughput(thoughtout), ThreadId(id), ConnectionNumber(connection) {}

    ThreadLoadData(const ThreadLoadData& item)
        : Throughput(item.Throughput),
          ThreadId(item.ThreadId),
          ConnectionNumber(item.ConnectionNumber) {}

    ThreadLoadData() {}  // 无锁队列不支持移动
};

template class LockFreeQueue<
    ThreadLoadData>;  // 模板定义，为了多个文件只生成一份实例，节省空间;

class LoadBalance : public Nocopy {
   public:
    explicit LoadBalance(ws::LockFreeQueue<ThreadLoadData>& que)
        : LoadBalanceQue(que), KeepData(false) {
        auto ThreadSum =
            std::max<int>(1, std::thread::hardware_concurrency() - 1);
        PerThreadThroughput.resize(ThreadSum);
        CurrentWeight.resize(ThreadSum, 0);

        PerThreadConnectNumber.resize(ThreadSum);
    }

    bool __attribute__((pure)) GetFlag() const noexcept { return KeepData; }
    void SetFlag(bool flag) noexcept { KeepData = flag; }

    size_t Distribution();
    size_t ExtractDataDromLockFreeQueue();

    LockFreeQueue<ThreadLoadData>& ReturnQue() const& noexcept {
        return LoadBalanceQue;
    }

   private:
    void Resize(ThreadLoadData& item);
    LockFreeQueue<ThreadLoadData>& LoadBalanceQue;

    // 基于吞吐量的负载均衡算法参考Nginx的负载均衡算法，需要下面三个值；
    std::vector<uint32_t> PerThreadThroughput;
    std::vector<long> CurrentWeight;  // 因为计算时会出现负数
    long TotalWeight;

    std::vector<uint16_t> PerThreadConnectNumber;
    bool KeepData;  // 是否保留前面计算的数据
};
}  // namespace ws

#endif