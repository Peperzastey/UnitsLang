#ifndef TKOMSIUNITS_CODE_OBJECTS_BOOL_VALUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BOOL_VALUE_H_INCLUDED

#include "Expression.h"

class BoolValue : public Expression {
public:
    BoolValue(bool value) : value_(value) {}
    
    /*const std::string& getInstrType() const override {
        static const std::string INSTR_TYPE = "BoolValue";
        return INSTR_TYPE;
    }*/
    
    std::string getRPN() const override {
        return value_ ? "true" : "false";
    }
    
private:
    bool value_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BOOL_VALUE_H_INCLUDED
