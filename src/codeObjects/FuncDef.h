#ifndef TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED

#include "InstructionBlock.h"
#include "Variable.h"
#include "Type.h"
#include "Value.h"
#include "error/ErrorHandler.h"
#include <memory>
#include <optional>
#include <set>
#include <vector>
#include <functional>
#include <string_view>

class FuncDef {
public:
    FuncDef(
            const std::string &name,
            std::vector<Variable> &&params,
            Type returnType,
            std::unique_ptr<InstructionBlock> &&body
        )
        : name_(name)
        , params_(std::move(params))
        , returnType_(std::move(returnType))
        , body_(std::move(body)) {
        std::set<std::string_view> paramNames;
        for (auto &&param : params_) {
            auto result = paramNames.insert(param.getName());
            if (!result.second) {
                ErrorHandler::handleFromCodeObject(
                    "Parameter duplicate (param name: '" + name + "') in definition of function '" + name_ + "'"
                );
            }
        }
    }
    
    std::optional<Value> call(Interpreter &interpreter, std::vector<Value> &&args) const;
    
    std::string toString() const {
        std::string output = name_ + '(';
        if (!params_.empty()) {
            output += params_.front().toString();
            for (auto it = params_.cbegin() + 1; it != params_.cend(); ++it) {
                output += ',' + it->toString();
            }  
        }
        output += ')';
        if (returnType_.getTypeClass() != Type::VOID) {
            output += "->" + returnType_.toString();
        }
        return output;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
    const Type& getType() const {
        return returnType_;
    }
    
private:
    const std::string name_;
    std::vector<Variable> params_;
    Type returnType_;
    std::unique_ptr<InstructionBlock> body_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_FUNC_DEF_H_INCLUDED
