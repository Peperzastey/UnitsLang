#ifndef TKOMSIUNITS_CODE_OBJECTS_CONTINUE_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_CONTINUE_H_INCLUDED

#include "Instruction.h"
#include <string>

class Continue : public Instruction {
public:
    Continue(/*While &parent*/) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "Continue";
        return INSTR_TYPE;
    }
    
private:
    //While &parent;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_CONTINUE_H_INCLUDED
