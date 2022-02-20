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
        );
    
    std::optional<Value> call(Interpreter &interpreter, std::vector<Value> &&args) const;
    
    std::string toString() const;
    
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
