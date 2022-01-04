#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED

#include "Expression.h"
#include <string>

class FuncCall : public Expression {
public:
    FuncCall(const std::string &name) : name_(name) {}

    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "FuncCall";
        return INSTR_TYPE;
    }

    const std::string& getName() const {
        return name_;
    }
    
    std::string getRPN() const override {
        return name_ + "()";
    }
    
private:
    std::string name_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_CALL_H_INCLUDED
