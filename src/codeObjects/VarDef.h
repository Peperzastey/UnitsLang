#ifndef TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED

#include "Instruction.h"
#include <string>

class VarDef : public Instruction {
public:
    VarDef(const std::string &name) : name_(name) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "VarDef";
        return INSTR_TYPE;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
