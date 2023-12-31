#include "loadbalance.h"

#include <limits.h>

#include <algorithm>
#include <iostream>
#include <numeric>

namespace ws {

extern template class LockFreeQueue<ThreadLoadData>;

size_t LoadBalance::Distribution() {
    // step1: 基于线程长连接数进行判断
    // nullptr

    // step2: 基于吞吐量进行判断
    for (size_t i = 0; i < CurrentWeight.size(); i++) {
        CurrentWeight[i] += PerThreadThroughput[i];
    }
    size_t index =
        std::max_element(CurrentWeight.begin(), CurrentWeight.end()) -
        CurrentWeight.begin();
    CurrentWeight[index] -= TotalWeight;

    return index;
}

// 从无锁队列中把数据取出来放到容器中;

size_t LoadBalance::ExtractDataDromLockFreeQueue() {
    if (!KeepData) {
        std::fill(PerThreadThroughput.begin(), PerThreadThroughput.end(), 0);
        std::fill(PerThreadConnectNumber.begin(), PerThreadConnectNumber.end(),
                  0);
    }

    auto len =
        LoadBalanceQue
            .size();  // 队列中至少len个项可读，如果多个线程读就要考虑取出的值为空了，单线程直接取就好；
    for (size_t i = 0; i < len; i++) {
        ThreadLoadData item;
        LoadBalanceQue.pop(item);  // 结构体开销比较小，直接拷贝即可
        if (!KeepData) {
            PerThreadConnectNumber[item.ThreadId] +=
                item.ConnectionNumber;  // 暂时不用
            PerThreadThroughput[item.ThreadId] += item.Throughput;
        } else {  // 数据保持的话要考虑数字越界
            Resize(item);
        }
    }

    // 用于基于吞吐量的负载均衡
    std::fill(CurrentWeight.begin(), CurrentWeight.end(), 0);

    TotalWeight =
        std::accumulate(PerThreadThroughput.begin(), PerThreadThroughput.end(),
                        0);  // 记录下来，少计算几次，32位先64位转，安全

    // 用于把吞吐量和线程的映射改变，因为本来这种负载均衡算法的初始值是基于机器的异构性的，我们需要反过来；
    std::vector<std::pair<uint32_t, size_t>> temp;
    for (size_t i = 0; i < PerThreadThroughput.size(); i++) {
        temp.emplace_back(std::make_pair(PerThreadThroughput[i], i));
    }

    sort(temp.begin(), temp.end());

    for (size_t i = 0; i < PerThreadThroughput.size() / 2; i++) {
        std::swap(PerThreadThroughput[temp[i].second],
                  PerThreadThroughput[temp[PerThreadThroughput.size() - i - 1]
                                          .second]);
    }

    return len;  // 返回无锁队列长度，暂时没什么用处
}

// 用于在KeepData为true的时候两个容器内数据越界；

void LoadBalance::Resize(
    ThreadLoadData&
        item) {  // 如果有一个特别大的数字传入就gg了，但是实际因为网卡的限制不会有那么大的数据
    if (USHRT_MAX - item.ConnectionNumber <
        PerThreadConnectNumber[item.ThreadId]) {
        std::for_each(PerThreadConnectNumber.begin(),
                      PerThreadConnectNumber.end(),
                      [](uint16_t& data) { data /= 2; });
    }
    PerThreadConnectNumber[item.ThreadId] += item.ConnectionNumber;

    if (UINT_MAX - item.Throughput < PerThreadThroughput[item.ThreadId]) {
        std::for_each(PerThreadThroughput.begin(), PerThreadThroughput.end(),
                      [](uint32_t& data) { data /= 2; });
    }

    PerThreadThroughput[item.ThreadId] += item.Throughput;
}

}  // namespace ws