#ifndef WS_H_
#define WS_H_

#include "../net/epoll.h"
#include "../tool/timer.h"
#include "channel.h"
#include "manger.h"
#include "server.h"
namespace ws {
class Web_Server {
   public:
    Web_Server();
    void Running();

   private:
    Manger _Manger_;
    Epoll _Epoll_;
    Server _Server_;
    Timer _Timer_;
};
}  // namespace ws

#endif