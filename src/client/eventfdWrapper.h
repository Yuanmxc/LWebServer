#ifndef EVENTFDWRAPPER_H_
#define EVENTFDWRAPPER_H_

#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <iostream>

#include "../base/havefd.h"

namespace ws {

class EventFdWrapper : public Havefd {
   private:
    int Eventfd_;

   public:
    EventFdWrapper()
        : Eventfd_(
              ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) {}

    int fd() const override { return Eventfd_; }

    void Read() {
        uint64_t howmany;
        ssize_t n = ::read(Eventfd_, &howmany, sizeof howmany);
        if (n != sizeof howmany) {
            std::cerr << "error in EventFdWrapper Read.\n";
        }
    }
};

}  // namespace ws

#endif  // EVENTFDWRAPPER_H_