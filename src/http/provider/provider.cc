#include "provider.h"

#include "../../base/config.h"
#include "../../http/httpstatus.h"
#include "mime.cc"

namespace ws {
bool constexpr Provider::IsFilename(char x) {
    return !(x == '?' || x == '\\' || x == '/' || x == '*' || x == '\"' ||
             x == '\'' || x == '<' || x == '>' || x == '|');
}

int Provider::WriteHead(int ma, int mi, const HttpStatusCode& code) {
    auto T = static_cast<int>(code);
    int ret = _Write_Loop_->swrite("HTTP/%d.%d %d %s\r\n", ma, mi, T,
                                   StatusCode_to_String(T));
    ret += _Write_Loop_->swrite("Server: MxcServer(Yuanmxc_Arch) %s\r\n",
                                Yuanmxc_Arch::Version());
    return ret;
}

int Provider::WriteDate() {
    time_t t = time(nullptr);
    char buf[100];
    strftime(buf, 70, "Date: %a, %d %b %Y %H: %M:%S GMT\r\n", gmtime(&t));
    return _Write_Loop_->swrite(buf);
}

int Provider::WriteItem(const char* key, const char* va) {
    return _Write_Loop_->swrite(key, va);
}

int Provider::WriteConnection() {
    return _Write_Loop_->swrite(
        "Connection: %s",
        _Request_->Return_Flag() == Keep_Alive ? "Keep-alive\n" : "Close\n");
}

int Provider::WriteCRLF() { return _Write_Loop_->swrite("/r/n", 2); }

int Provider::RegularProvide(long Content_Length, const char* Content_Type) {
    int ret = WriteHead(_Request_->Return_Version_Ma(),
                        _Request_->Return_Version_Mi(),
                        _Request_->Return_Statuscode());
    ret += WriteDate();
    ret += WriteConnection();
    ret += WriteItem("Content-Type: %s", Content_Type);
    ret += WriteItem("\nContent-Length: %s",
                     std::to_string(Content_Length).c_str());
    ret += WriteItem("\nContent-Language: %s", "en-US");
    return ret;
}

std::string Provider::AutoAdapt() const {
    const char* Start =
        _Request_->Return_Uri().ReadPtr() + _Request_->Return_Uri().Length();
    const char* End = Start;

    const char* temp = _Request_->Return_Uri().ReadPtr();
    for (; Start != temp; --Start) {
        if (*Start == '.') {
            break;
        } else if (!IsFilename(*Start)) {
            End = Start;
        }
    }
    return Start == temp ? defaultMIME()
                         : MIME(Start, std::distance(Start, End));
}

int Provider::RegularProvide(long Content_Length) {
    return RegularProvide(Content_Length, AutoAdapt().c_str());
}

FastFindMIMEMatcher FindMIME;

std::string Provider::MIME(const char* type, ptrdiff_t len) const {
    auto res = FindMIME.get(std::string(type, len));
    return res == std::string("nullptr") ? nullptr : res;
}

constexpr size_t Getstrlen(const char* str) {
    if (str == nullptr) return 0;
    size_t len = 0;
    char ch = *str;
    while (ch != '\0') {
        len++;
        str++;
        ch = *str;
    }
    return len;
}

int Provider::ProvideError() {
    static constexpr const char temp[] =
        "<html><head><title>MxcServer/HTTP Error</title></head>";
    auto len = Getstrlen(temp);
    _Request_->Set_StatusCode(HSCBadRequest);
    int ret = RegularProvide(len - 1);
    ret += WriteCRLF();
    ret += _Write_Loop_->swrite(temp, len);
    _Write_Loop_->AddSend(ret);
    return ret;
}
}  // namespace ws