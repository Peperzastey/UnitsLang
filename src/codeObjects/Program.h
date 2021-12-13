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
    Program(std::vector<std::unique_ptr<Instruction>> &&instructions) {
        for (auto &&instr : instructions) {
            if ( FuncDef *funcDef = dynamic_cast<FuncDef*>(instr.get()) ) {
                std::string name = funcDef->getName();
                instr.release();
                addFuncDef(name, std::unique_ptr<FuncDef>(funcDef));
            } else {
                instructions_.push_back(std::move(instr));
            }
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
    void addFuncDef(const std::string &name, std::unique_ptr<FuncDef> funcDef) {
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
