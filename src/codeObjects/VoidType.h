#ifndef TKOMSIUNITS_CODE_OBJECTS_VOID_TYPE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VOID_TYPE_H_INCLUDED

#include "Type.h"
#include <string>

class VoidType : public Type {
public:
    VoidType() {}
    
    std::string toString() const override {
        return "";
    }
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VOID_TYPE_H_INCLUDED
