
#ifndef YUANMXC_ARCH_H_
#define YUANMXC_ARCH_H_

#include <memory>

#include "../http/httprequest.h"
#include "../net/address.h"

namespace Yuanmxc_Arch {

constexpr char* Version() { return (char*)"0.0"; }

constexpr int MyPort();

constexpr char* MyIP();

constexpr int FastCgiPort();

constexpr char* FastCgiIP();

constexpr int EventResult_Number();
}  // namespace Yuanmxc_Arch

#endif