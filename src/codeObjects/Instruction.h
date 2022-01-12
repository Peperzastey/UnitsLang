#ifndef TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED

#include "InstrResult.h"
#include <string>

class Interpreter;

class Instruction {
public:
    virtual ~Instruction() = 0;
    
    virtual InstrResult execute(Interpreter &interpreter) const = 0;
    virtual const std::string& getInstrType() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
