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
    
    std::string getRPN() const override;

private:
    std::optional<Value> doCall(Interpreter &interpreter) const;

private:
    const std::string name_;
    const std::vector<std::unique_ptr<Expression>> args_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
