#ifndef TKOMSIUNITS_CODE_OBJECTS_UNIT_BINARY_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_UNIT_BINARY_EXPRESSION_H_INCLUDED

#include "UnitExpression.h"
#include "lexer/Lexer.h"
#include <memory>
#include <set>

namespace codeobj {

class UnitBinaryExpression : public UnitExpression {
public:
    UnitBinaryExpression(std::unique_ptr<UnitExpression> leftOperand,
               Token op,
               std::unique_ptr<UnitExpression> rightOperand)
        : leftOperand_(std::move(leftOperand))
        , rightOperand_(std::move(rightOperand))
        , operator_(op) {}

    std::string getRPN() const override {
        std::ostringstream os;
        os << leftOperand_->getRPN() << rightOperand_->getRPN() << std::get<std::string>(operator_.value);
        return os.str();
    }

private:
    std::unique_ptr<UnitExpression> leftOperand_;
    std::unique_ptr<UnitExpression> rightOperand_;
    Token operator_;    //TODO Operator class from Parser world
    std::set<::Unit> numerator_;
    std::set<::Unit> denominator_;
};

}

#endif // TKOMSIUNITS_CODE_OBJECTS_UNIT_BINARY_EXPRESSION_H_INCLUDED
