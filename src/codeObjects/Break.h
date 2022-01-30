#ifndef TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED

#include "Instruction.h"
#include <string>

class Break : public Instruction {
public:
    Break() {}
    
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override {
        return InstrResult::BREAK;
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "Break";
        return INSTR_TYPE;
    }
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED
