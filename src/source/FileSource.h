#ifndef TKOMSIUNITS_FILESOURCE_H_INCLUDED
#define TKOMSIUNITS_FILESOURCE_H_INCLUDED

#include "Source.h"
#include <string>
#include <fstream>

class FileSource : public Source {
public:
    FileSource(const std::string &filename);

private:
    char provideChar();

private:
    std::string filename_;
    std::ifstream file_;
};

#endif // TKOMSIUNITS_FILESOURCE_H_INCLUDED
