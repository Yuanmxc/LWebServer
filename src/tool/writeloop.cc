#include "writeloop.h"

#include <sys/socket.h>

#include <cstdarg>

namespace ws {
int WriteLoop::swrite(const char* format, ...) {
    va_list va;
    va_start(va, format);
    int ret = User_Buffer_->SWrite(format, va);
    va_end(va);
    write(ret);
    return ret;
}

WriteLoop::COMPLETETYPE WriteLoop::Send(int length) {
    if (length < 1)
        throw std::invalid_argument("'WriteLoop::Send' error parameter.");
    int sent_ = 0;
    // 有一次发送失败就退出循环，原因是内核写入队列已满，不应该空转等待
    for (int ans; length - sent_ &&
                  (ans = static_cast<int>(send(fd_, User_Buffer_->ReadPtr(),
                                               length - sent_, 0))) > 0;) {
        sent_ += ans;
    }
    // 一个http响应报文至少需要两次send，所以缓冲区需要记录长度；
    User_Buffer_->read(sent_);
    int Remaining = length - sent_;
    bool faultError = false;
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
        if (errno == EPIPE || errno == ECONNRESET) {
            // 服务端收到RST报文后recv出现ECONNRESET;向一个已关闭的套接字写数据的时候出现EPIPE；
            // 此时显然我们没有必要再去维护发送缓冲区了，直接返回true就可以了，因为这个套接字已经无效了；
            faultError = true;
        }
    }
    if (!faultError && Remaining > 0) {
        InsertSend(Remaining);
        return IMCOMPLETE;
    }
    return COMPLETE;
}

WriteLoop::COMPLETETYPE WriteLoop::SendFile(std::shared_ptr<FileReader> ptr) {
    ssize_t len = 0;
    while (len = ptr->SendFile(fd_) && len > 0) {
    }
    if (!ptr->Send_End()) {
        InsertSendFile(ptr);
        return IMCOMPLETE;
    }
    return COMPLETE;
}

WriteLoop::COMPLETETYPE WriteLoop::DoFirst() {
    if (!Que.empty()) {
        auto Fun = Que.front();
        Que.pop_front();
        return Fun();
    }
    return EMPTY;
}

WriteLoop::COMPLETETYPE WriteLoop::DoAll() {
    while (1) {
        auto CompleteType = DoFirst();
        if (CompleteType == COMPLETE)
            continue;
        else
            return CompleteType;
    }
}

}  // namespace ws