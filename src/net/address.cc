#include "address.h"

#include <arpa/inet.h>

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace ws {
Address::Address(const char* IP, int port) : addr_{} {
    assert(port > 0);

    if (inet_pton(AF_INET, IP, &(addr_.sin_addr)) <= 0) {
        // 处理IP地址转换失败的情况
        throw std::runtime_error("Invalid IP address");
    }

    addr_.sin_port = htons(static_cast<uint16_t>(port));
    addr_.sin_family = AF_INET;
}

Address::Address(int port) : addr_{} {
    assert(port > 0);

    if (inet_pton(AF_INET, "127.0.0.1", &(addr_.sin_addr)) <= 0) {
        // 处理IP地址转换失败的情况
        throw std::runtime_error("Invalid IP address");
    }

    addr_.sin_port = htons(static_cast<uint16_t>(port));
    addr_.sin_family = AF_INET;
}
}  // namespace ws
