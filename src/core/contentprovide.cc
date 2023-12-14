#include "contentprovide.h"

#include "provider/reaprovider.h"

namespace ws {

void ContentProvider::Provide() {
    // 延迟绑定，创建的时候数据还没准备好
    if (!Pro) Pro = std::make_unique<REAProvider>(Http_Request, Write_Loop);
    Pro->provide();
}

}  // namespace ws