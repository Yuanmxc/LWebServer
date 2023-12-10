#ifndef YUANMXC_ARCH_H_
#define YUANMXC_ARCH_H_

#include <memory>

#include "../http/httprequest.h"
#include "../net/address.h"

namespace Yuanmxc_Arch {

constexpr char* Version() noexcept { return (char*)"0.0"; }

constexpr int MyPort() noexcept { return 8888; }

constexpr char* MyIP() noexcept { return (char*)"127.0.0.1"; }

constexpr int FastCgiPort() noexcept;

constexpr char* FastCgiIP() noexcept;

constexpr int EventResult_Number() noexcept { return 8096; }

}  // namespace Yuanmxc_Arch

#endif