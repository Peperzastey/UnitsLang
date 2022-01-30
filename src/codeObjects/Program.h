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
    Program(std::vector<std::unique_ptr<FuncDef>> &&funcDefs,
            std::vector<std::unique_ptr<Instruction>> &&instructions)
            : instructions_(std::move(instructions)) {
        addPredefinedPrintFunc();
        for (auto &&func : funcDefs) {
            addFuncDef(std::move(func));
        }
    }
    
    int execute(Interpreter &interpreter) const;
    
    const FuncDef* getFuncDef(const std::string &name) const {
        auto iter = funcDefs_.find(name);
        return (iter != funcDefs_.cend() ? iter->second.get() : nullptr);
    }

private:
    void addFuncDef(std::unique_ptr<FuncDef> funcDef) {
        std::string name = funcDef->getName();
        auto [_, success] = funcDefs_.insert({ name, std::move(funcDef) });
        (void)_;
        if (!success) {
            std::ostringstream os;
            os << "Redefinition of function named '" << name << "'";
            ErrorHandler::handleFromCodeObject(os.str());
        }
    }
    
    void addPredefinedPrintFunc() {
        std::vector<std::unique_ptr<Instruction>> printInstructions;
        printInstructions.emplace_back(std::make_unique<InternalPrintInstr>("_"));
        auto printFuncInstrBlock = std::make_unique<InstructionBlock>(
                std::move(printInstructions)
            );
        auto printFuncDef = std::make_unique<FuncDef>(
                "print",
                std::vector{ Variable{"_", Type::STRING} },
                Type::VOID,
                std::move(printFuncInstrBlock)
            );
        addFuncDef(std::move(printFuncDef));
    }

private:
    std::unordered_map<std::string, std::unique_ptr<FuncDef>> funcDefs_;
    InstructionBlock instructions_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED
