#ifndef TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED

#include "InstructionBlock.h"
#include "Expression.h"
#include "error/ErrorHandler.h"
#include <memory>
#include <vector>
#include <string>

class If : public Instruction {
public:
    If(
        std::unique_ptr<Expression> cond,
        std::unique_ptr<InstructionBlock> positiveBlock,
        std::unique_ptr<If> elseIf
        )
        : cond_(std::move(cond))
        , positiveBlock_(std::move(positiveBlock))
        , elseIf_(std::move(elseIf)) {}

    // Else
    If(std::unique_ptr<InstructionBlock> positiveBlock)
        : cond_(nullptr)
        , positiveBlock_(std::move(positiveBlock))
        , elseIf_(nullptr) {}
    
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override {
        if (cond_) {
            Value condResult = cond_->calculate(interpreter);
            if (condResult.type.getTypeClass() != Type::BOOL) {
                ErrorHandler::handleTypeMismatch("Expression used as If condition must result in bool value");
            }
            if (condResult.asBool()) {
                // True
                return positiveBlock_->execute(interpreter);
            } else if (elseIf_) {
                // False
                return elseIf_->execute(interpreter);
            }
            return InstrResult::NORMAL;
        } else {
            // Else
            return positiveBlock_->execute(interpreter);
        }
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "If";
        return INSTR_TYPE;
    }
    
private:
    std::unique_ptr<Expression> cond_;
    std::unique_ptr<InstructionBlock> positiveBlock_;
    std::unique_ptr<If> elseIf_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED
