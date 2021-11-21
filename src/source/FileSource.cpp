#include "FileSource.h"
#include <stdexcept>

FileSource::FileSource(const std::string &filename)
    : filename_(filename)
    , file_(filename_, std::ios::in) {
    if (!file_.is_open()) {
        std::string errorMsg = "Cannot open file " + filename_;
        throw std::runtime_error(errorMsg);
    }
}

char FileSource::provideChar() {
    return file_.get();
}
