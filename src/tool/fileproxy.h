#ifndef FILEPROXY_H_
#define FILEPROXY_H_

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "../base/havefd.h"
#include "../base/nocopy.h"

namespace ws {

class FileProxy : public Nocopy, public Havefd {
   public:
    explicit FileProxy(const char* path)
        : File_Description(::open(path, O_RDONLY)) {}
    FileProxy(const FileProxy&, const char*);
    ~FileProxy() override;

    int fd() & const noexcept final { return File_Description; }
    bool Fd_Good() const noexcept { return File_Description > -1; }
    __off_t FileSize();
    bool IsTextFile();

   private:
    int File_Description;
    std::unique_ptr<struct stat> stat_ = nullptr;
    void Statget();
};

}  // namespace ws

#endif