#ifndef TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED

#include <string>

class String {
public:
    String(std::string &&value) : value_(std::move(value)) {}
    
    std::string toString() const {
        return value_;
    }

private:
    std::string value_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED
