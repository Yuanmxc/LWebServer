#ifndef CONTENTPROVIDER_H_
#define CONTENTPROVIDER_H_

#include <memory>

#include "../base/nocopy.h"
#include "../http/httprequest.h"
#include "../net/writeloop.h"
#include "provider/provider.h"
#include "provider/reaprovider.h"

namespace ws {

class ContentProvider : public Nocopy {
   public:
    ContentProvider(std::shared_ptr<HttpRequest> Hr,
                    std::shared_ptr<WriteLoop> Wl)
        : Http_Request(Hr), Write_Loop(Wl) {}
    void Provide();

   private:
    std::unique_ptr<Provider> Pro;
    std::shared_ptr<HttpRequest> Http_Request;
    std::shared_ptr<WriteLoop> Write_Loop;
};
}  // namespace ws

#endif