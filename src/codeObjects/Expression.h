#ifndef TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED

#include "Instruction.h"
#include "lexer/Lexer.h"
#include <string>

class Value;
class Interpreter;

class Expression {
public:
    Expression() {}
    virtual ~Expression() {}
    
    virtual Value calculate([[maybe_unused]] Interpreter &interpreter) = 0;
    virtual std::string getRPN() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_EXPRESSION_H_INCLUDED
