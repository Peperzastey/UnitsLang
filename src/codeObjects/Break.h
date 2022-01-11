#ifndef TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED

#include "Instruction.h"
#include <string>

class Break : public Instruction {
public:
    Break() {}
    
    InstrResult execute() override {
        return InstrResult::BREAK;
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "Break";
        return INSTR_TYPE;
    }
    
private:
    //While &parent;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_BREAK_H_INCLUDED
