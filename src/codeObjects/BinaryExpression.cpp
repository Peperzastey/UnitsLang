#include "BinaryExpression.h"

namespace {

void assertNumberTypes(const Value &left, const Value &right, const std::string &opName) {
    if (left.type.getTypeClass() != Type::NUMBER || right.type.getTypeClass() != Type::NUMBER) {
        ErrorHandler::handleTypeMismatch(opName + " operands must be of numeric type");
    }
}
void assertBoolTypes(const Value &left, const Value &right, const std::string &opName) {
    if (left.type.getTypeClass() != Type::BOOL || right.type.getTypeClass() != Type::BOOL) {
        ErrorHandler::handleTypeMismatch(opName + " operands must be of bool type");
    }
}
void assertNumberOrBoolTypes(const Value &left, const Value &right, const std::string &opName) {
    if ((left.type.getTypeClass() != Type::NUMBER && left.type.getTypeClass() != Type::BOOL)
        || (right.type.getTypeClass() != Type::NUMBER && right.type.getTypeClass() != Type::BOOL)
    ) {
        ErrorHandler::handleTypeMismatch(opName + " operands must be of numeric or bool type");
    }
}
void assertEqualTypes(const Value &left, const Value &right, const std::string &opName) {
    if (left.type != right.type) {
        ErrorHandler::handleTypeMismatch(opName + " operands are not type-compatibile");
    }
}

template <typename BinaryFunc>
void additiveOp(Value &left, const Value &right, BinaryFunc &&func, const std::string &opName) {
    assertNumberTypes(left, right, opName);
    assertEqualTypes(left, right, opName);
    left.value = func(left.asDouble(), right.asDouble());
}
template <typename BinaryFunc>
void multiplicativeOp(Value &left, const Value &right, BinaryFunc &&func, const std::string &opName) {
    assertNumberTypes(left, right, opName);
    left.value = func(left.asDouble(), right.asDouble());
}
template <typename BinaryFunc>
void relativeOp(Value &left, const Value &right, BinaryFunc &&func, const std::string &opName) {
    additiveOp(left, right, std::forward<BinaryFunc>(func), opName);
    left.type = Type::BOOL;
}
template <typename BinaryFunc>
void equalityOp(Value &left, const Value &right, BinaryFunc &&func, const std::string &opName) {
    assertNumberOrBoolTypes(left, right, opName);
    assertEqualTypes(left, right, opName);
    if (left.type.getTypeClass() == Type::BOOL) {
        left.value = func(left.asBool(), right.asBool());
    } else {
        left.value = func(left.asDouble(), right.asDouble());
    }
    left.type = Type::BOOL;
}
template <typename BinaryFunc>
void logicalOp(Value &left, const Value &right, BinaryFunc &&func, const std::string &opName) {
    assertBoolTypes(left, right, opName);
    assertEqualTypes(left, right, opName);
    left.value = func(left.asBool(), right.asBool());
}

void add(Value &left, const Value &right) {
    additiveOp(left, right, std::plus<double>{}, "Addition");
}
void subtract(Value &left, const Value &right) {
    additiveOp(left, right, std::minus<double>{}, "Subtraction");
}
void mult(Value &left, const Value &right) {
    multiplicativeOp(left, right, std::multiplies<double>{}, "Multiplication");
    left.type.asUnit().multWithUnit(right.type.asUnit());
}
void div(Value &left, const Value &right) {
    multiplicativeOp(left, right, std::divides<double>{}, "Division");
    left.type.asUnit().divWithUnit(right.type.asUnit());
}
void greaterThan(Value &left, const Value &right) {
    relativeOp(left, right, std::greater<double>{}, "GreaterThan");
}
void greaterThanOrEqual(Value &left, const Value &right) {
    relativeOp(left, right, std::greater_equal<double>{}, "GreaterThanOrEqual");
}
void lessThan(Value &left, const Value &right) {
    relativeOp(left, right, std::less<double>{}, "LessThan");
}
void lessThanOrEqual(Value &left, const Value &right) {
    relativeOp(left, right, std::less_equal<double>{}, "LessThanOrEqual");
}
void equalTo(Value &left, const Value &right) {
    equalityOp(left, right, std::equal_to<>{}, "EqualTo");
}
void notEqualTo(Value &left, const Value &right) {
    equalityOp(left, right, std::not_equal_to<>{}, "NotEqualTo");
}
void logicAnd(Value &left, const Value &right) {
    logicalOp(left, right, std::logical_and<>{}, "And");
}
void logicOr(Value &left, const Value &right) {
    logicalOp(left, right, std::logical_or<>{}, "Or");
}

} // anonymous namespace

Value BinaryExpression::calculate([[maybe_unused]] Interpreter &interpreter) {
    static const std::unordered_map<std::string, void(*)(Value &, const Value &)> operations {
        {"+" , &add},
        {"-" , &subtract},
        {"*" , &mult},
        {"/" , &div},
        {">" , &greaterThan},
        {">=" , &greaterThanOrEqual},
        {"<" , &lessThan},
        {"<=" , &lessThanOrEqual},
        {"==", &equalTo},
        {"!=", &notEqualTo},
        {"&&", &logicAnd},
        {"||", &logicOr}
    };
    Value left = leftOperand_->calculate(interpreter);
    Value right = rightOperand_->calculate(interpreter);
    operations.at(std::get<std::string>(operator_.value))(left, right);
    return left;
}
