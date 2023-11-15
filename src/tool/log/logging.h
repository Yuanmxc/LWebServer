#ifndef LOGGING_H_
#define LOGGING_H_

#include <string.h>

#include <functional>
#include <string>

namespace ws {

namespace detail {

class logging {
   public:
    enum Loglevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class Filewrapper {
       public:
        template <int Num>
        explicit Filewrapper(const char (&arr)[Num])  // 匹配char数组
            : data_(arr), size_(N - 1) {
            const char* point = strrchr(data_, '/');
            if (point) {
                data_ = point + 1;
                size_ -= std::distance(arr, data_);
            }
        }

        explicit Filewrapper(const std::string& filename) : data_(filename) {
            const char* point = strrchr(data_, '/');
            if (point) {
                data_ = point + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

       private:
        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    using OutputFun = std::function<void(const char*, int)>;
    using FlushFun = std::function<void(void)>;
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

   private:
    class Funwrapper {
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;

        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Funwrapper wrapper_;
};

extern Logger::LogLevel g_logLevel

    inline Logger::LogLevel
    Logger::logLevel() {
    return g_logLevel;
}

}  // namespace detail

}  // namespace ws

#endif  // LOGGING_H_