#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <assert.h>

#include <thread>

namespace ws {

class EventLoop {
   private:
    bool looping;
    const std::thread::id threadID;

   public:
    EventLoop();
    ~EventLoop();
    void loop();
};

}  // namespace ws

#endif  // EVENTLOOP_H_