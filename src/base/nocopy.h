#ifndef NOCOPY_H_
#define NOCOPY_H_

namespace ws {
class
    Nocopy {  // 作为一个基类，表示它的派生类对象不能进行拷贝构造和拷贝赋值操作。
   public:
    Nocopy(const Nocopy&) = delete;
    Nocopy& operator=(const Nocopy& para) = delete;

   protected:
    constexpr Nocopy() = default;
    virtual ~Nocopy() = default;
};
}  // namespace ws

#endif