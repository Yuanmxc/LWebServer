#ifndef REAPROVIDER_H_
#define REAPROVIDER_H_

#include <memory>

#include "../../http/httprequest.h"
#include "../../tool/filereader.h"
#include "../../tool/writeloop.h"
#include "../parsed_header.h"
#include "provider.h"

namespace ws {

class REAProvider : public Provider {
   public:
    REAProvider(std::shared_ptr<HttpRequest> Hr, std::shared_ptr<WriteLoop> Wl)
        : Provider(Hr, Wl) {}
    void provide() final;

   private:
    bool FileProvider(std::shared_ptr<FileReader>& file);
};

}  // namespace ws

#endif