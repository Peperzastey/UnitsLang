#ifndef TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include <memory>
#include <vector>
#include <string>

class If : public Instruction {
public:
    If(
        std::unique_ptr<Expression> cond,
        std::vector<std::unique_ptr<Instruction>> &&positiveBlock,
        //std::vector<std::unique_ptr<Instruction>> negativeBlock, //else == else if (true)
        std::unique_ptr<If> elseIf //or vec of ElseIf
        )
        : cond_(std::move(cond))
        , positiveBlock_(std::move(positiveBlock))
        , elseIf_(std::move(elseIf)) {}

    // Else
    If(std::vector<std::unique_ptr<Instruction>> &&positiveBlock)
        : cond_(nullptr)
        , positiveBlock_(std::move(positiveBlock))
        , elseIf_(nullptr) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "If";
        return INSTR_TYPE;
    }
    
private:
    std::unique_ptr<Expression> cond_;
    std::vector<std::unique_ptr<Instruction>> positiveBlock_;
    std::unique_ptr<If> elseIf_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_IF_H_INCLUDED
