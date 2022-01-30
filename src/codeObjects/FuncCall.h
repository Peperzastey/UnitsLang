#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include "Value.h"
#include "Interpreter.h"
#include "FuncDef.h"
#include <string>

class FuncCall : public Instruction, public Expression {
public:
    FuncCall(
            const std::string &name,
            std::vector<std::unique_ptr<Expression>> &&args
        )
        : name_(name)
        , args_(std::move(args)) {}

    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override {
        doCall(interpreter);
        return InstrResult::NORMAL;
    }
    
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        std::optional<Value> retVal = doCall(interpreter);
        if (!retVal) {
            ErrorHandler::handleTypeMismatch("Function call as expression cannot evaluate to type void");
        }
        return retVal.value();
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
    std::optional<Value> doCall(Interpreter &interpreter) const {
        const FuncDef *funcDef = interpreter.getFuncDef(name_);

        std::vector<Value> argVals;
        argVals.reserve(args_.size());
        for (auto &&arg : args_) {
            argVals.push_back(arg->calculate(interpreter));
        }
        
        return funcDef->call(interpreter, std::move(argVals));
    }

private:
    const std::string name_;
    const std::vector<std::unique_ptr<Expression>> args_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
