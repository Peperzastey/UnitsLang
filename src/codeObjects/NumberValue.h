#ifndef TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED

#include "Expression.h"
#include "Unit.h"
#include "lexer/Lexer.h"
#include "utils/printUtils.h"
#include <memory>
#include <sstream>

class NumberValue : public Expression {
public:
    NumberValue(double value, codeobj::Unit &&unit)
        : value_(value), unit_(std::move(unit)) {}
    
    /*const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "NumberValue";
        return INSTR_TYPE;
    }*/
    
    std::string getRPN() const override {
        std::ostringstream os;
        os << value_;
        if (!unit_.isScalar()) {
            os << unit_;
        }
        return os.str();
    }
    
private:
    double value_;
    codeobj::Unit unit_; //TODO Unit struct from Parser world
};

#endif // TKOMSIUNITS_CODE_OBJECTS_NUMBER_VALUE_H_INCLUDED
