#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED

#include "Instruction.h"
#include <memory>
#include <vector>
#include <string>

class FuncDef {
public:
    FuncDef(const std::string &name, std::vector<std::unique_ptr<Instruction>> &&body)
        : name_(name), body_(std::move(body)) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "FuncDef";
        return INSTR_TYPE;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
    std::vector<std::unique_ptr<Instruction>> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
