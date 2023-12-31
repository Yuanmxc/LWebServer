#ifndef TIMER_H_
#define TIMER_H_

#include <sys/timerfd.h>
#include <unistd.h>

#include "../base/havefd.h"
#include "../base/nocopy.h"

namespace ws {
class Timer : public Havefd, public Nocopy {
   public:
    Timer()
        : timeFd(
              ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)) {}
    int fd() const& noexcept override { return timeFd; }

    // 间隔时间与首次超时时间，单位为纳秒;
    // 默认为200ms以后启动，100ms间隔触发一次；
    int SetTimer(long intervalNanoseconds = 1000000000l,
                 long firstNanoseconds = 2000000000l) {
        struct itimerspec its;

        constexpr long base = 1000000000;

        its.it_interval.tv_sec =
            intervalNanoseconds / base;  // 后面触发的间隔时间
        its.it_interval.tv_nsec = intervalNanoseconds % base;

        its.it_value.tv_sec = firstNanoseconds / base;  // 首次超时时间
        its.it_value.tv_nsec = firstNanoseconds % base;
        if (::timerfd_settime(timeFd, 0, &its, nullptr) < 0) return -1;
        return 0;  // timerfd_settime成功时返回0，等于-1的时候失败；
    }

    ~Timer() { ::close(timeFd); }

   private:
    int timeFd;
};
}  // namespace ws

#endif