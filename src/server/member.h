#ifndef MEMBER_H_
#define MEMBER_H_

#include <algorithm>
#include <memory>

#include "../base/havefd.h"
#include "../base/nocopy.h"
#include "../http/httpparser.h"
#include "../http/httprequest.h"
#include "../http/provider/contentprovide.h"
#include "../net/socket.h"
#include "../tool/userbuffer.h"
#include "../tool/writeloop.h"

#ifndef __GNUC__

#define __attribute__(x) /*NOTHING*/

#endif

namespace ws {
class Member : public Nocopy, public Havefd {
   public:
    explicit Member(int fd)
        : Socket_Ptr(std::make_unique<Socket>(fd)), WriteComplete(false) {
        Init();
    }

    explicit Member(std::unique_ptr<Socket>&& ptr) : WriteComplete(false) {
        Init();
        std::swap(Socket_Ptr, ptr);
    }
    Member() = delete;

    void DoRead();
    void DoWrite();
    bool CloseAble() const&;

    int fd() const& noexcept final { return Socket_Ptr->fd(); }
    void Init();

    void clear();

    bool __attribute__((pure)) IsWriteComplete() const noexcept {
        return WriteComplete;
    }

    int InitiativeClose() {  // 用于主动关闭套接字
        return Socket_Ptr->Close();
    }

    ~Member() { Socket_Ptr->Close(); }

   private:
    // 用于优化Fd_To_Member.erase操作，此操作极其耗费时间，占到了CPU总占比的百分之十左右，引入clear，做一个类似slab的缓存；

    std::unique_ptr<Socket> Socket_Ptr;

    std::shared_ptr<UserBuffer> User_Buffer;
    std::unique_ptr<HttpParser> Http_Parser_;
    std::shared_ptr<HttpRequest> Http_Request_;
    std::shared_ptr<WriteLoop> Write_Loop_;
    std::unique_ptr<ContentProvider> Content_Provider_;

    bool WriteComplete;
};
}  // namespace ws

#endif