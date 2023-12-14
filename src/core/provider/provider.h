#ifndef PROVIDER_H_
#define PROVIDER_H_

#include <iostream>
#include <memory>

#include "../../base/copyable.h"
#include "../../http/httprequest.h"
#include "../../http/httpstatus.h"
#include "../../net/writeloop.h"

namespace ws {
class Provider : public Copyable {
   public:
    Provider(std::shared_ptr<HttpRequest> Hr, std::shared_ptr<WriteLoop> Wl)
        : _Request_(Hr), _Write_Loop_(Wl) {}
    int WriteHead(int, int, const HttpStatusCode&);
    int WriteItem(const char*, const char*);
    int WriteConnection();
    int WriteDate();
    int WriteCRLF();

    int ProvideError();
    int RegularProvide(long Content_Length);
    int RegularProvide(long Content_Length, const char*);
    // long int
    std::string MIME(const char*, ptrdiff_t) const;
    std::string AutoAdapt() const;  // 用于指定响应数据的类型和编码
    bool Good() const { return _Request_->Request_good(); }

    static std::string defaultMIME() { return "text/html\n"; }
    static constexpr bool IsFilename(char);

    virtual void provide() = 0;

   protected:
    std::shared_ptr<HttpRequest> _Request_;
    std::shared_ptr<WriteLoop> _Write_Loop_;
};
}  // namespace ws
#endif