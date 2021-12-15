#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED

#include <string>

class FuncDef {
public:
    FuncDef(const std::string &name) : name_(name) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "FuncDef";
        return INSTR_TYPE;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
