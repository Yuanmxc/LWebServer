#include "member.h"

#include <memory>

#include "../tool/userbuffer.h"

namespace ws {
void Member::DoRead() {
    // step1: 从套接字中把数据读到Member的User_Buffer和Socket的Extrabuf中；
    Socket_Ptr->Read(User_Buffer);

    // step2:做一些解析http请求的前置工作(主要是上一次可能解析过，但是数据包不够大，导致解析失败)；
    if (Http_Parser_->Finished()) {  // 检测报文是否大于最小的有效HTTP长度
        Http_Parser_->Again_Parser();  // 做一些解析前的预处理工作，
    }

    // step3: 开始http解析,并把响应头和文件内容全部加入到输出缓冲区中；
    Http_Parser_->Starting_Parser();
    if (Http_Parser_->Finished()) {
        Content_Provider_->Provide();
    }

    // step4: 将输出缓冲区内容发出去
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
    User_Buffer = std::make_shared<UserBuffer>(4048);
    Http_Request_ = std::make_shared<HttpRequest>();
    Http_Parser_ = std::make_unique<HttpParser>(
        User_Buffer, Http_Request_, Socket_Ptr->ReturnExtraBuffer());
    Write_Loop_ = std::make_shared<WriteLoop>(fd(), 4048);
    Content_Provider_ =
        std::make_unique<ContentProvider>(Http_Request_, Write_Loop_);
}

bool Member::CloseAble() const& {
    if (Http_Parser_->Finished() &&
        ((Http_Request_->Return_Version_Ma() == 1 &&
          Http_Request_->Return_Version_Mi() == 0) ||
         Http_Request_->Return_Flag() != Keep_Alive))
        return true;  // http1.0不支持短连接
    return false;
}
}  // namespace ws