#ifndef TKOMSIUNITS_CODE_OBJECTS_STR_TYPE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_STR_TYPE_H_INCLUDED

#include "Type.h"
#include <string>

class StrType : public Type {
public:
    StrType() {}
    
    /*TypeClass getTypeClass() const override {
        return TypeClass::STRING;
    }*/
    
    std::string toString() const override {
        return "[str]";
    }
};

#endif // TKOMSIUNITS_CODE_OBJECTS_STR_TYPE_H_INCLUDED
