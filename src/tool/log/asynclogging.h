#ifndef ASYNCLOGGING_H_
#define ASYNCLOGGING_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../../base/nocopy.h"
#include "Fixedbuffer.h"

namespace ws {

namespace detail {
class AsyncLogging : public Nocopy {
   public:
    AsyncLogging(const std::string& basename, off_t rollsize,
                 size_t flushInterval = 3);

    void start();
    void stop();
    void append(const char* line, int len);

    ~AsyncLogging() {
        if (running) {
            stop();
        }
    }

   private:
    void AsyncFunc();

    using Buffer = FixedBuffer<LargerBuffer>;
    using Buffervector = std::vector<std::unique_ptr<Buffer>>;
    using Bufferptr = typename Buffervector::value_type;

    const std::string basename;
    const off_t rollsize_;
    const size_t flushInterval_;

    std::thread thread_;
    std::atomic_bool running;
    std::mutex mutex_;
    std::condition_variable cv;
    Bufferptr currentBuffer_;
    Bufferptr nextBuffer_;
    Buffervector buffers_;
};

}  // namespace detail

}  // namespace ws

#endif  // ASYNCLOGGING_H_