#ifndef TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_BLOCK_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_BLOCK_H_INCLUDED

#include "Instruction.h"
#include <memory>
#include <string>
#include <vector>

class Interpreter;

class InstructionBlock {
public:
    InstructionBlock(std::vector<std::unique_ptr<Instruction>> &&instructions)
        : instructions_(std::move(instructions)) {}
    
    InstrResult execute(Interpreter &interpreter) const;
    
private:
    const std::vector<std::unique_ptr<Instruction>> instructions_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INSTRUCTION_BLOCK_H_INCLUDED
