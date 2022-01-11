#ifndef TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include <memory>
#include <vector>
#include <string>

class While : public Instruction {
public:
    While(
        std::unique_ptr<Expression> cond,
        std::vector<std::unique_ptr<Instruction>> &&body
        )
        : cond_(std::move(cond))
        , body_(std::move(body)) {}
        
    InstrResult execute() override {
        //TODO
        return InstrResult::NORMAL;
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "While";
        return INSTR_TYPE;
    }
    
private:
    std::unique_ptr<Expression> cond_;
    std::vector<std::unique_ptr<Instruction>> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_WHILE_H_INCLUDED
