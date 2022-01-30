#ifndef TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED

#include "Expression.h"
#include "Type.h"
#include <variant>
#include <sstream>

struct Value : public Expression {
    Value(double value, Type &&type)
        : value(value), type(std::move(type)) {
        if (type.getTypeClass() != Type::NUMBER) {
            ErrorHandler::handleFromCodeObject("Number value with type not NUMBER");
        }        
    }
    Value(bool value)
        : value(value), type(Type::BOOL) {}
    Value(std::string value)
        : value(std::move(value)), type(Type::STRING) {}
        
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        return *this;
    }
    
    std::string getRPN() const override {
        return toString();
    }
    
    std::string toString() const {
        std::ostringstream os;

        if (std::holds_alternative<double>(value)) {
            os << asDouble();
            const codeobj::Unit &unit = type.asUnit();
            if (!unit.isScalar()) {
                os << unit;
            }
        } else if (std::holds_alternative<bool>(value)) {
            os << (asBool() ? "true" : "false");
        } else { // string
            os << asString();
        }

        return os.str();
    }
    
    double asDouble() const {
        return std::get<double>(value);
    }
    
    bool asBool() const {
        return std::get<bool>(value);
    }
    
    const std::string& asString() const {
        return std::get<std::string>(value);
    }
    
    bool operator==(const Value &other) const {
        return value == other.value && type == other.type;
    }
    
    bool operator!=(const Value &other) const {
        return !(*this == other);
    }
    
    std::variant<double, bool, std::string> value;
    Type type;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VALUE_H_INCLUDED
