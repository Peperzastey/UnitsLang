#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include <string>

class FuncCall : public Instruction, public Expression {
public:
    FuncCall(
            const std::string &name,
            std::vector<std::unique_ptr<Expression>> &&args
        )
        : name_(name)
        , args_(std::move(args)) {}

    InstrResult execute() override {
        //TODO
        return InstrResult::NORMAL;
    }

    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "FuncCall";
        return INSTR_TYPE;
    }

    const std::string& getName() const {
        return name_;
    }
    
    std::string getRPN() const override {
        std::string output = name_ + '(';
        if (!args_.empty()) {
            output += args_.front()->getRPN();
            for (auto it = args_.cbegin() + 1; it != args_.cend(); ++it) {
                output += ',' + (*it)->getRPN();
            }  
        }
        output += ')';
        return output;
    }
    
private:
    const std::string name_;
    const std::vector<std::unique_ptr<Expression>> args_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
