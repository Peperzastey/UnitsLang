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
        
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override;
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "While";
        return INSTR_TYPE;
    }

private:
    std::unique_ptr<Expression> cond_;
    std::unique_ptr<InstructionBlock> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED
