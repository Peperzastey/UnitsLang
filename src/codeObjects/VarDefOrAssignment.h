#ifndef TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include "Type2.h"
#include "Value.h"
#include "Interpreter.h"
#include <memory>
#include <optional>
#include <string>

class VarDefOrAssignment : public Instruction {
public:
    VarDefOrAssignment(const std::string &name,
        std::unique_ptr<Expression> expr,
        std::optional<Type2> declatedType
        )
        : name_(name)
        , expr_(std::move(expr))
        , declaredType_(std::move(declatedType)) {}
    
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override {
        Value value = expr_->calculate(interpreter);

        if (declaredType_) {
            // this is variable definition
            if (declaredType_ != value.type) {
                ErrorHandler::handleTypeMismatch("Expression result type does not match declared variable type in variable '" + name_ + "' definition");
            }
            interpreter.addVariable(name_, std::move(value));
        } else if (auto val = interpreter.getVariable(name_); !val) {
            // this is variable definition without type declaration
            interpreter.addVariable(name_, std::move(value));
        } else {
            // this is variable assignment
            if (val->type != value.type) {
                ErrorHandler::handleTypeMismatch("Expression result type does not match variable type in variable '" + name_ + "' assignment");
            }
            interpreter.assignVariable(name_, std::move(value));
        }
        
        return InstrResult::NORMAL;
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "VarDefOrAssignment";
        return INSTR_TYPE;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
    std::unique_ptr<Expression> expr_;
    std::optional<Type2> declaredType_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
