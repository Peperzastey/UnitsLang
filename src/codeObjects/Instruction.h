#ifndef TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED

#include <string>

class Instruction {
public:
    virtual ~Instruction() = 0;
    
    virtual const std::string& getInstrType() const = 0;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_H_INCLUDED
