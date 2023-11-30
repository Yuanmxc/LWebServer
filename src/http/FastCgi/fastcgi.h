#ifndef FASTCGI_H_
#define FASTCGI_H_

#include "fastcgiHeader.h"

namespace ws {

class FastCgi {
   private:
   public:
    FCGI_Header makeHeader(int type, int request, int contentLength,
                           int paddingLength);
};

}  // namespace ws

#endif  // FASTCGI_H_