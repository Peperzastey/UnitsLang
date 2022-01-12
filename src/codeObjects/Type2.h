#ifndef TKOMSIUNITS_CODE_OBJECTS_TYPE2_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_TYPE2_H_INCLUDED

#include "Unit.h"
#include <cassert>
#include <string>
#include <variant>

class Type2 {
public:
    enum TypeClass {
        NUMBER,
        BOOL,
        VOID,
        STRING
    };
    
public:
    Type2(TypeClass typeClass = TypeClass::VOID) : unit_(), type_(typeClass) {}
    Type2(codeobj::Unit &&unit) : unit_(std::move(unit)), type_(NUMBER) {}
    
    //Type2(Type2 &&) = default;
    //Type2& operator=(Type2 &&) = default;

    TypeClass getTypeClass() const noexcept {
        return type_;
    }
    
    const codeobj::Unit& asUnit() const {
        assert(type_ == NUMBER);
        return unit_;
    }
    
    std::string toString() const {
        switch (type_) {
            case NUMBER:
                return unit_.toString();
            case BOOL:
                return "[bool]";
            case VOID:
                return "[void]";
            case STRING:
                return "[str]";
        }
    }
    
    bool operator==(const Type2 &other) const {
        return type_ == other.type_ && unit_.isAddCompatibileWith(other.unit_);
    }
    
    bool operator!=(const Type2 &other) const {
        return !(*this == other);   
    }
    /*bool isUnit() const {
        return std::holds_alternative<codeobj::Unit>(value);
    }
    bool isScalar() const {
        return isUnit() && std::get<codeobj::Unit>(value).isScalar();
    }
    bool isBool() const {
        return std::holds_alternative<bool>(value);
    }
    bool isVoid() const {
        return std::holds_alternative<std::monostate>(value);
    }
    bool isString() const {
        return std::holds_alternative<std::string>(value);
    }*/
    
private:
    //std::variant<std::monostate, codeobj::Unit, bool, std::string> value;
    codeobj::Unit unit_;
    TypeClass type_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_TYPE2_H_INCLUDED
