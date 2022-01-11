#ifndef TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED

#include "Type.h"
#include <string>
#include <memory>

class Variable {
public:
    Variable(const std::string &name, std::unique_ptr<Type> type)
        : name_(name), type_(std::move(type)) {}
    
    Variable(Variable &&) = default;
    Variable& operator=(Variable &&) = default;
    
    const std::string& getName() const {
        return name_;
    }
    
    std::string toString() const {
        return name_ + type_->toString();
    }
    
private:
    std::string name_;
    std::unique_ptr<Type> type_;
    //std::variant<double, int> value_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VARIABLE_H_INCLUDED
