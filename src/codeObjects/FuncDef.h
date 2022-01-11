#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED

#include "Instruction.h"
#include "Variable.h"
#include "VoidType.h"
#include "Type2.h"
#include "error/ErrorHandler.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

class FuncDef {
public:
    FuncDef(
            const std::string &name,
            std::vector<Variable> &&params,
            Type2 returnType,
            std::vector<std::unique_ptr<Instruction>> &&body
        )
        : name_(name)
        , returnType_(std::move(returnType))
        , body_(std::move(body)) {
        for (auto &&param : params) {
            std::string name = param.getName();
            auto [iter, success] = params_.insert({ name, std::move(param) });
            if (!success) {
                ErrorHandler::handleFromCodeObject(
                    "Parameter duplicate (param name: '" + name + "') in definition of function '" + name_ + "'"
                );
            }
            paramsOrder_.push_back(iter->second);
        }
    }
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "FuncDef";
        return INSTR_TYPE;
    }
    
    std::string toString() const {
        std::string output = name_ + '(';
        if (!paramsOrder_.empty()) {
            output += paramsOrder_.front().get().toString();
            for (auto it = paramsOrder_.cbegin() + 1; it != paramsOrder_.cend(); ++it) {
                output += ',' + it->get().toString();
            }  
        }
        output += ')';
        if (returnType_.getTypeClass() != Type2::VOID) {
            output += "->" + returnType_.toString();
        }
        return output;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
    const Type2& getType() const {
        return returnType_;
    }
    
private:
    const std::string name_;
    std::unordered_map<std::string, Variable> params_;
    std::vector<std::reference_wrapper<const Variable>> paramsOrder_;
    Type2 returnType_;
    std::vector<std::unique_ptr<Instruction>> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
