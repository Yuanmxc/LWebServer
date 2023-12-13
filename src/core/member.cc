#include "member.h"

#include <memory>

#include "../tool/userbuffer.h"

namespace ws {
void Member::DoRead() {
    // step1: 从套接字中把数据读到Member的User_Buffer和Socket的Extrabuf中；
    Socket_Ptr->Read(User_Buffer);

    // step2: 做一些解析http请求的前置工作；
    if (Http_Parser_->Finished()) {  // 检测报文是否大于最小的有效HTTP长度
        Http_Parser_->Again_Parser();  // 做一些解析前的预处理工作，
    }

    // step3: 开始http解析
    Http_Parser_->Starting_Parser();
    if (Http_Parser_->Finished()) {
        Content_Provider_->Provide();
    }
    if (Write_Loop_->DoAll() != WriteLoop::IMCOMPLETE) {
        WriteComplete = true;
    }
}

void Member::DoWrite() {
    if (Write_Loop_->DoAll() != WriteLoop::IMCOMPLETE) {
        WriteComplete = true;
    } else {
        WriteComplete = false;
    }
}

void Member::Init() {
    User_Buffer = std::make_shared<UserBuffer>(4096);
    Http_Request_ = std::make_shared<HttpRequest>();
    Http_Parser_ = std::make_unique<HttpParser>(
        User_Buffer, Http_Request_, Socket_Ptr->ReturnExtraBuffer());
    Write_Loop_ = std::make_shared<WriteLoop>(fd(), 4096);
    Content_Provider_ =
        std::make_unique<ContentProvider>(Http_Request_, Write_Loop_);
}

bool Member::CloseAble() const {
    if (Http_Request_->Return_Flag() != Keep_Alive && Http_Parser_->Finished())
        return true;
    return false;
}
}  // namespace ws