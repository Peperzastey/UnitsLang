#ifndef TKOMSIUNITS_STRINGSOURCE_H_INCLUDED
#define TKOMSIUNITS_STRINGSOURCE_H_INCLUDED

#include "Source.h"
#include <string>

class StringSource : public Source {
public:
    StringSource(const std::string &str);

private:
    char provideChar();

private:
    const std::string chars_;
    std::size_t pos_;
};

#endif // TKOMSIUNITS_STRINGSOURCE_H_INCLUDED
