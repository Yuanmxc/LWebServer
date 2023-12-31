#ifndef WRITELOOP_H_
#define WRITELOOP_H_

#include <deque>
#include <functional>
#include <memory>
#include <string>

#include "../base/havefd.h"
#include "../base/nocopy.h"
#include "../tool/filereader.h"
#include "../tool/userbuffer.h"

namespace ws {

class WriteLoop : public Nocopy, public Havefd {
   public:
    enum COMPLETETYPE { IMCOMPLETE, COMPLETE, EMPTY };
    using Task = std::function<WriteLoop::COMPLETETYPE()>;

    WriteLoop(int fd, std::function<void(int)> fun, int length = 4096)
        : fd_(fd),
          WriteLoopCallback(fun),
          User_Buffer_(std::make_unique<UserBuffer>(length)) {}
    int fd() const& override { return fd_; }

    int write(int bytes) { return User_Buffer_->Write(bytes); }
    int write(char* buf, int bytes) { return User_Buffer_->Write(buf, bytes); }
    int write(const char* buf, int bytes) {
        return User_Buffer_->Write(buf, bytes);
    }
    int write(const std::string& str) { return User_Buffer_->Write(str); }
    int swrite(const char* format, ...);

    int writeable() const { return User_Buffer_->Writeable(); }
    void Move_Buffer() { User_Buffer_->Move_Buffer(); }
    size_t WSpot() const noexcept { return User_Buffer_->WSpot(); }
    void Rewrite(int spot) noexcept { return User_Buffer_->ReWirte(spot); }

    void AddSend(int length) {
        Que.emplace_back([this, length] { return Send(length); });
    }
    void AddSend() {
        Que.emplace_back([this] { return Send(User_Buffer_->Readable()); });
    }
    void AddSendFile(std::shared_ptr<FileReader> ptr) {
        Que.emplace_back([this, ptr] { return SendFile(ptr); });
    }

    COMPLETETYPE DoFirst();
    COMPLETETYPE DoAll();

    [[deprecated]] size_t TaskQueSize() const& noexcept {
        return Que.size();
    }  // For Debugging.

    // 复用member对象套接字会变，但是fd不变
    void clear() {
        Que.clear();
        User_Buffer_->Clean();
    }

   private:
    std::unique_ptr<UserBuffer> User_Buffer_;
    std::deque<Task> Que;  // 支持长连接
    int fd_;
    std::function<void(int)> WriteLoopCallback;
    uint32_t throughout = 0;
    // 负载均衡发送间隔，即调用多少次Dofirst以后发送到全局队列中，主要为了减少队列长度，以3为间隔，因为一个http响应报文最少需要调用三次
    uint32_t interval;
    static const uint32_t expectedInetrval = 3;

    COMPLETETYPE Send(int length);
    COMPLETETYPE SendFile(std::shared_ptr<FileReader>);
    void InsertSend(int len) {
        Que.emplace_front([this, len] { return Send(len); });
    }
    void InsertSendFile(const std::shared_ptr<FileReader>& ptr) {
        Que.emplace_back([this, ptr] { return SendFile(ptr); });
    }
};
}  // namespace ws

#endif