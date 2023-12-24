#ifndef HTTPPARSER_H_
#define HTTPPARSER_H_

#include <memory>

#include "../base/nocopy.h"
#include "../net/socket.h"
#include "../tool/userbuffer.h"
#include "httprequest.h"
#include "httpstatus.h"

#ifndef __GNUC__

#define __attribute__(x) /*NOTHING*/

#endif

namespace ws {

class HttpParser : public Nocopy {
   public:
    HttpParser(std::shared_ptr<UserBuffer> ptr,
               std::shared_ptr<HttpRequest> request, Extrabuf* extra)
        : User_Buffer_(std::move(ptr)),
          Parser_Result(std::make_unique<HttpParser_Content>()),
          Request_Result(request),
          Extrabuffer_(extra) {}

    void __attribute__((hot)) Again_Parser();
    HttpParserFault __attribute__((hot)) Starting_Parser();
    bool __attribute__((hot)) Finished() const {
        return Parser_Result->Fault == HPFContent;
    }

    bool SetRequesting();

    void clear() {
        User_Buffer_->Clean();
        Parser_Result->init();
        Request_Result->clear();
        // Extrabuffer_在socket clear的时候已经初始化了
    }

   private:
    std::shared_ptr<UserBuffer> User_Buffer_;
    std::unique_ptr<HttpParser_Content> Parser_Result;
    std::shared_ptr<HttpRequest> Request_Result;
    Extrabuf* Extrabuffer_;
    bool Parsering();
    bool Parser_able() __attribute__((pure)) {
        return User_Buffer_->Readable() >= 16;
    }  // 请求行+空行，最小的HTTP方法为三字节
};

}  // namespace ws

#endif