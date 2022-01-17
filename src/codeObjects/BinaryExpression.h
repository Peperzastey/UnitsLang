#ifndef TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED

#include "Expression.h"
#include "Value.h"
#include "lexer/Lexer.h"
#include "error/ErrorHandler.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sstream>

inline void add(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("Addition operands must be of numeric type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("Addition operands are not type-compatibile");
    }
    //TODO units can have different prefixes! -- actualise value and type
    left.value = left.asDouble() + right.asDouble();
}

inline void minus(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("Addition operands must be of numeric type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("Addition operands are not type-compatibile");
    }
    //TODO units can have different prefixes! -- actualise value and type
    left.value = left.asDouble() - right.asDouble();
}

inline void mult(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("Multiplication operands must be of numeric type");
    }
    left.value = left.asDouble() * right.asDouble();
    left.type.asUnit().multWithUnit(right.type.asUnit());
}

inline void div(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("Multiplication operands must be of numeric type");
    }
    left.value = left.asDouble() / right.asDouble();
    left.type.asUnit().divWithUnit(right.type.asUnit());
}

inline void greaterThan(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("GreaterThan operands must be of numeric type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("GreaterThan operands are not type-compatibile");
    }
    //TODO units can have different prefixes!
    left.value = left.asDouble() > right.asDouble();
    left.type = Type::BOOL;
}

inline void lessThan(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch("LessThan operands must be of numeric type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("LessThan operands are not type-compatibile");
    }
    //TODO units can have different prefixes!
    left.value = left.asDouble() < right.asDouble();
    left.type = Type::BOOL;
}

inline void equalTo(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER && left.type.getTypeClass() != Type::BOOL) {
        ErrorHandler::handleTypeMismatch("EqualTo operands must be of numeric or bool type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("EqualTo operands are not type-compatibile");
    }
    //TODO units can have different prefixes!
    left.value = left.asDouble() == right.asDouble();
    left.type = Type::BOOL;
}

inline void notEqualTo(Value &left, const Value &right) {
    if (left.type.getTypeClass() != Type::NUMBER && left.type.getTypeClass() != Type::BOOL) {
        ErrorHandler::handleTypeMismatch("NotEqualTo operands must be of numeric or bool type");
    }
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch("NotEqualTo operands are not type-compatibile");
    }
    //TODO units can have different prefixes!
    left.value = left.asDouble() != right.asDouble();
    left.type = Type::BOOL;
}

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> leftOperand,
               Token op,
               std::unique_ptr<Expression> rightOperand)
        : leftOperand_(std::move(leftOperand))
        , rightOperand_(std::move(rightOperand))
        , operator_(op) {}
    
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        static const std::unordered_map<std::string, void(*)(Value &, const Value &)> operations {
            {"+" , &add},
            {"-" , &minus},
            {"*" , &mult},
            {"/" , &div},
            {">" , &greaterThan},
            {"<" , &lessThan},
            {"==", &equalTo},
            {"!=", &notEqualTo}
        };
        Value left = leftOperand_->calculate(interpreter);
        Value right = rightOperand_->calculate(interpreter);
        operations.at(std::get<std::string>(operator_.value))(left, right);
        return left;
    }

    std::string getRPN() const override {
        std::ostringstream os;
        os << leftOperand_->getRPN() << rightOperand_->getRPN() << std::get<std::string>(operator_.value);
        return os.str();
    }

private:
    std::unique_ptr<Expression> leftOperand_;
    std::unique_ptr<Expression> rightOperand_;
    Token operator_;    //TODO Operator class from Parser world
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED
