#include "logging.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <cstdio>
#include <sstream>

namespace ws {

namespace detail {

thread_local char t_errnoBuf[512];
thread_local char t_time[64];
thread_local time_t t_lastSecond;

const char* strerror_tl(int savedErrno) {
    return strerror_r(savedErrno, t_errnoBuf, sizeof(t_errnoBuf));
}

constexpr Logger::LogLevel initLogLevel() {
    if (::getenv("LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

constexpr Logger::LogLevel g_logLevel = initLogLevel();

constexpr const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL ",
};

class helper {
   public:
    helper(const char* str, unsigned len) : str_(str), len_(len) {
        assert(strlen(str) == len_);
    }

    const char* str_;
    const unsigned len_;
};

void defaultOutput(const char* msg, int len) {
    size_t n = fwrite(msg, 1, len, stdout);
    (void)n;
}

void defaultFlush() { fflush(stdout); }

logging::OutputFun g_output(defaultOutput);
logging::FlushFun g_flush(defaultFlush);

}  // namespace detail

}  // namespace ws