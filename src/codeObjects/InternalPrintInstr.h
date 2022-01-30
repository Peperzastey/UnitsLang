#ifndef TKOMSIUNITS_CODE_OBJECTS_INTERNAL_PRINT_INSTR_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INTERNAL_PRINT_INSTR_H_INCLUDED

#include "Instruction.h"

class InternalPrintInstr : public Instruction {
public:
    InternalPrintInstr(const std::string &stringVariableName)
        : stringVariableName_(stringVariableName) {}

    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override;
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "InternalPrintInstr";
        return INSTR_TYPE;
    }

private:
    std::string stringVariableName_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INTERNAL_PRINT_INSTR_H_INCLUDED
