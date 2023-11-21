#include <fcntl.h>
#include <unistd.h>
#include <memory>

#include "fileproxy.h"

namespace ws {

void FileProxy::Statget() {
    if (!stat_) {
        stat_ = std::make_unique<struct stat>();
        fstat(File_Description, stat_.get());
    }
}

__off_t FileProxy::FileSize() {
    Statget();
    return stat_->st_size;
}

bool FileProxy::IsTextFile() {
    Statget();
    return (stat_->st_mode & S_IFDIR || stat_->st_mode & S_IFCHR ||
            stat_->st_mode & S_IFBLK);
}

FileProxy::~FileProxy() { ::close(File_Description); }

FileProxy::FileProxy(const FileProxy& path1, const char* path2)
    : File_Description(openat(path1.fd(), path2, O_RDONLY)) {}

}  // namespace ws