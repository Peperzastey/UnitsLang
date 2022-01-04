#ifndef TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED

#include "Expression.h"
#include "lexer/Lexer.h"
#include <string>
#include <memory>
#include <sstream>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> leftOperand,
               Token op,
               std::unique_ptr<Expression> rightOperand)
        : leftOperand_(std::move(leftOperand))\
        , rightOperand_(std::move(rightOperand))
        , operator_(op) {}
    
    const std::string& getInstrType() const override {
        static const std::string INSTR_TYPE = "BinaryExpression";
        return INSTR_TYPE;
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
