#include <gperftools/profiler.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "server/ws.h"

#undef __GNUC__

// 用于服务端程序执行性能测试
void setGperfStatus(int signum) {
    static bool is_open = false;
    if (signum != SIGUSR1) {
        return;
    }
    if (!is_open) {  // start
        is_open = true;
        ProfilerStart("test.prof");
        std::cout << "ProfilerStart success" << std::endl;
    } else {  // stop
        is_open = false;
        ProfilerStop();
        std::cout << "ProfilrerStop success" << std::endl;
    }
}

int main(int argc, char **argv) {
    signal(SIGUSR1, setGperfStatus);
    std::cout << "pid = " << getpid()
              << std::endl;  // 通过发送请求来开始和终止性能测试
    ws::Web_Server().Running();
    return 0;
}