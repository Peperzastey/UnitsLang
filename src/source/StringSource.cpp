#include "StringSource.h"
#include <stdexcept>

StringSource::StringSource(const std::string &str)
    : chars_(str)
    , pos_(0) {
}

char StringSource::provideChar() {
    if (pos_ >= chars_.size()) {
        return EOF;
    }
    return chars_[pos_++];
}
