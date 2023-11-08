
#ifndef YUANMXC_ARCH_H_
#define YUANMXC_ARCH_H_

#include <memory>

#include "../http/httprequest.h"
#include "../net/address.h"

namespace Yuanmxc_Arch {

constexpr char* Version() { return (char*)"0.0"; }

int MyPort();

int EventResult_Number();
}  // namespace Yuanmxc_Arch

#endif