#ifndef TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED

#include "Expression.h"
#include <sstream>

class NumberValue : public Expression {
public:
    NumberValue(double value) : value_(value) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "NumberValue";
        return INSTR_TYPE;
    }
    
    std::string getRPN() const override {
        std::ostringstream os;
        os << value_;
        return os.str();
    }
    
private:
    double value_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED
