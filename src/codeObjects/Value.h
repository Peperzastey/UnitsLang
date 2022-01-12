#ifndef TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED

#include "Expression.h"
#include "Type2.h"
#include <variant>
#include <sstream>

struct Value : public Expression {
    Value(double value, Type2 &&type)
        : value(value), type(std::move(type)) {
        if (type.getTypeClass() != Type2::NUMBER) {
            ErrorHandler::handleFromCodeObject("Number value with type not NUMBER");
        }        
    }
    Value(bool value)
        : value(value), type(Type2::BOOL) {}
        
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        return *this;
    }
    
    std::string getRPN() const override {
        return toString();
    }
    
    std::string toString() const {
        std::ostringstream os;

        if (std::holds_alternative<double>(value)) {
            os << std::get<double>(value);
            const codeobj::Unit &unit = type.asUnit();
            if (!unit.isScalar()) {
                os << unit;
            }
        } else {
            os << (std::get<bool>(value) ? "true" : "false");
        }

        return os.str();
    }
    
    double asDouble() const {
        return std::get<double>(value);
    }
    
    bool asBool() const {
        return std::get<bool>(value);
    }
    
    bool operator==(const Value &other) const {
        return value == other.value && type == other.type;
    }
    
    bool operator!=(const Value &other) const {
        return !(*this == other);
    }
    
    std::variant<double, bool> value;
    Type2 type;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED
