#ifndef TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED

#include <string>

enum class InstrResult {
    NORMAL,
    RETURN, // +value -- set in stack in ExecutionContext(/Interpreter)?
    BREAK,
    CONTINUE
    //EXCEPTION ? // +exception type
};

class Instruction {
public:
    virtual ~Instruction() = 0;
    
    virtual InstrResult execute() = 0;
    virtual const std::string& getInstrType() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
