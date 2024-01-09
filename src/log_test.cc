#include <bits/stdc++.h>
#include <unistd.h>

#include "log/asynclogging.h"
#include "log/logfile.h"
#include "log/logging.h"

using namespace std;

std::unique_ptr<ws::detail::AsyncLogging> g_logFile;

void outputFunc(const char* msg, int len) { g_logFile->append(msg, len); }

int main(int argc, char* argv[]) {
    char name[256] = {0};
    strncpy(name, argv[0], sizeof name - 1);
    g_logFile.reset(new ws::detail::AsyncLogging(::basename(name), 200 * 1000));
    g_logFile->start();  // 开启异步线程
    std::string line =
        "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for (int i = 0; i < 2000000; ++i) {
        ws::detail::log_INFO(__FILE__, __LINE__, errno).stream()
            << line << i << ":"
            << "\n";

        // usleep(1000);
    }
}