#ifndef TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED

#include "Instruction.h"
#include "lexer/Lexer.h"
#include <string>

class Expression {
public:
    Expression() {}
    virtual ~Expression() {}
    
    /*const std::string& getInstrType() const override {
        static const std::string INSTR_TYPE = "Expression";
        return INSTR_TYPE;
    }*/
    
    virtual std::string getRPN() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED
