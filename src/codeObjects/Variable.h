#ifndef TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED

#include "Type.h"
#include <string>
#include <memory>

class Variable {
public:
    Variable(const std::string &name, Type type)
        : name_(name), type_(std::move(type)) {}
    
    const std::string& getName() const {
        return name_;
    }
    
    const Type& getType() const {
        return type_;
    }
    
    std::string toString() const {
        return name_ + type_.toString();
    }
    
private:
    std::string name_;
    Type type_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED
