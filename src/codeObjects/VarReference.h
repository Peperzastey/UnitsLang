#ifndef TKOMSIUNITS_CODE_OBJECTS_VAR_REFERENCE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VAR_REFERENCE_H_INCLUDED

#include "Expression.h"
#include "Value.h"
#include "Interpreter.h"
#include <string>

class VarReference : public Expression {
public:
    VarReference(const std::string &name) : name_(name) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "VarReference";
        return INSTR_TYPE;
    }
    
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        return interpreter.getVariableOrError(name_);
    }
    
    const std::string& getName() const {
        return name_;
    }
    
    std::string getRPN() const override {
        return name_;
    }
    
private:
    std::string name_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VAR_REFERENCE_H_INCLUDED
