#ifndef TKOMSIUNITS_CODE_OBJECTS_BOOL_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BOOL_H_INCLUDED

#include "Type.h"
#include <string>

class Bool : public Type {
public:
    Bool() {}
    
    /*TypeClass getTypeClass() const override {
        return TypeClass::BOOL;
    }*/
    
    std::string toString() const override {
        return "[bool]";
    }
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BOOL_H_INCLUDED
