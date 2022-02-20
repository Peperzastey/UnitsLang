#ifndef TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED

#include "InstructionBlock.h"
#include "InternalPrintInstr.h"
#include "FuncDef.h"
#include "Value.h"
#include "error/ErrorHandler.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <sstream>

#include <iostream>

class Interpreter;

class Program {
public:
    Program(
            std::vector<std::unique_ptr<FuncDef>> &&funcDefs,
            std::vector<std::unique_ptr<Instruction>> &&instructions
        );
    
    int execute(Interpreter &interpreter) const;
    
    const FuncDef* getFuncDef(const std::string &name) const;

private:
    void addFuncDef(std::unique_ptr<FuncDef> funcDef);
    void addPredefinedPrintFunc();

private:
    std::unordered_map<std::string, std::unique_ptr<FuncDef>> funcDefs_;
    InstructionBlock instructions_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED
