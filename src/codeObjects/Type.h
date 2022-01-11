#ifndef TKOMSIUNITS_CODE_OBJECTS_TYPE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_TYPE_H_INCLUDED

#include <string>

class Type {
public:
    enum TypeClass {
        UNIT,
        BOOL,
        VOID,
        STRING
    };

public:
    Type() {}
    virtual ~Type() {}
    
    virtual std::string toString() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_TYPE_H_INCLUDED
