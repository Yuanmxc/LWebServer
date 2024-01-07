#include "parsed_header.h"

#include <assert.h>

#include <iostream>

#ifndef __GNUC__

#define __attribute__(x) /*NOTHING*/

#endif
namespace ws {
constexpr inline int __attribute__((always_inline))
my_memcmp(const void *buffer1, const void *buffer2, int count) throw() {
    if (!count) return (0);
    while (--count && *(char *)buffer1 == *(char *)buffer2) {
        buffer1 = (char *)buffer1 + 1;
        buffer2 = (char *)buffer2 + 1;
    }
    return (*((unsigned char *)buffer1) - *((unsigned char *)buffer2));
}

constexpr inline int __attribute__((always_inline))
my_strlen(const char *str) throw() {
    const char *p = str;
    while (*p) {
        p++;
    }
    return p - str;
}

bool ParsedHeader::ParsedHeaderIsEqual(const ParsedHeader &para) const {
    if (length != para.Readable()) return false;
    if (my_memcmp(Header, para.ReadPtr(), length) == 0) return true;
    return false;
}

bool ParsedHeader::ParsedHeaderIsEqual(const char *ptr) const {
    if (length != my_strlen(ptr)) return false;
    if (my_memcmp(Header, ptr, length) == 0) return true;
    return false;
}

bool ParsedHeader::ParsedHeaderIsEqual(const std::string &str) const {
    if (length != str.length()) return false;
    if (my_memcmp(Header, str.c_str(), length) == 0) return true;
    return false;
}

std::ostream &operator<<(std::ostream &os, const ParsedHeader &ptr) {
    os.write(ptr.ReadPtr(), ptr.Readable());
    return os;
}

bool ParsedHeader::operator==(const ParsedHeader &para) const {
    bool Len_is_equal = (length == para.Readable());
    return (Len_is_equal && (strncmp(Header, para.ReadPtr(), length) == 0));
}
}  // namespace ws