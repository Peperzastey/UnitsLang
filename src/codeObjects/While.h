#ifndef TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED

#include "InstructionBlock.h"
#include "Expression.h"
#include "Interpreter.h"
#include <memory>
#include <vector>
#include <string>

class While : public Instruction {
public:
    While(
        std::unique_ptr<Expression> cond,
        std::unique_ptr<InstructionBlock> body
        )
        : cond_(std::move(cond))
        , body_(std::move(body)) {}
        
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override {
        auto calcCond = [&interpreter, cond = cond_.get()]() {
                Value condResult = cond->calculate(interpreter);
                if (condResult.type.getTypeClass() != Type2::BOOL) {
                    ErrorHandler::handleTypeMismatch("Expression used as While condition must result in bool value");
                }
                return condResult.asBool();
            };
        auto notBreakOrReturn = [](InstrResult result) {
                switch (result) {
                    case InstrResult::BREAK:
                    case InstrResult::RETURN:
                        return false;
                    default:
                        return true;
                }
            };
        InstrResult result = InstrResult::NORMAL;

        for (bool cond = calcCond(); cond && notBreakOrReturn(result); cond = calcCond()) {
            result = body_->execute(interpreter);
        }
        
        if (result == InstrResult::BREAK || result == InstrResult::CONTINUE) {
            result = InstrResult::NORMAL;
        }
        return result;
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "While";
        return INSTR_TYPE;
    }

private:
    std::unique_ptr<Expression> cond_;
    std::unique_ptr<InstructionBlock> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED
