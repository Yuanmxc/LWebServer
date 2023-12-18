#include "address.h"

#include <strings.h>

#include <cassert>

namespace ws {
Address::Address(const char* IP, int port) {
    assert(port > 0);

    bzero(&addr_, sizeof(addr_));

    addr_.sin_addr.s_addr = inet_addr(IP);
    addr_.sin_port = htons(static_cast<uint16_t>(port));
    addr_.sin_family = AF_INET;
}

Address::Address(int port) {
    assert(port > 0);

    bzero(&addr_, sizeof(addr_));

    addr_.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_.sin_port = htons(static_cast<uint16_t>(port));
    addr_.sin_family = AF_INET;
}
}  // namespace ws