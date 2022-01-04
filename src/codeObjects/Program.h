#ifndef TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED

#include "Instruction.h"
#include "FuncDef.h"
#include "error/ErrorHandler.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <sstream>

#include <iostream>

class Program {
public:
    Program(std::vector<std::unique_ptr<FuncDef>> &&funcDefs,
            std::vector<std::unique_ptr<Instruction>> &&instructions)
            : instructions_(std::move(instructions)) {
        for (auto &&func : funcDefs) {
            addFuncDef(std::move(func));
        }
    }
    
    void printInstructionsTypes() const {
        std::cout << "Num of instrs: " << instructions_.size()
            << "\nInstr types:\n";
        for (const auto &instr : instructions_) {
            std::cout << "* " << instr->getInstrType() << '\n';
        }
    }

    void printFunctions() const {
        std::cout << "Num of func defs: " << funcDefs_.size()
            << "\nFunc names:\n";
        for (const auto &entry : funcDefs_) {
            std::cout << "* " << entry.first << '\n';
        }
    }

private:
    void addFuncDef(std::unique_ptr<FuncDef> funcDef) {
        std::string name = funcDef->getName();
        auto [_, success] = funcDefs_.insert({ name, std::move(funcDef) });
        (void)_;
        if (!success) {
            std::ostringstream os;
            os << "Redefinition of function named '" << name << "'";
            ErrorHandler::handleFromParser(os.str());
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<FuncDef>> funcDefs_;
    std::vector<std::unique_ptr<Instruction>> instructions_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_PROGRAM_H_INCLUDED
