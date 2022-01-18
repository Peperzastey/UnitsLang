#ifndef TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED

#include "Expression.h"
#include "Value.h"
#include "lexer/Lexer.h"
#include "error/ErrorHandler.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sstream>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> leftOperand,
               Token op,
               std::unique_ptr<Expression> rightOperand)
        : leftOperand_(std::move(leftOperand))
        , rightOperand_(std::move(rightOperand))
        , operator_(op) {}
    
    Value calculate([[maybe_unused]] Interpreter &interpreter) override;

    std::string getRPN() const override {
        std::ostringstream os;
        os << leftOperand_->getRPN() << rightOperand_->getRPN() << std::get<std::string>(operator_.value);
        return os.str();
    }

private:
    std::unique_ptr<Expression> leftOperand_;
    std::unique_ptr<Expression> rightOperand_;
    Token operator_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BINARY_EXPRESSION_H_INCLUDED
