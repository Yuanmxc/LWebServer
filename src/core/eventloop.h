#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <assert.h>

#include <thread>

#include "../base/nocopy.h"
namespace ws {

class EventLoop : public Nocopy {
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