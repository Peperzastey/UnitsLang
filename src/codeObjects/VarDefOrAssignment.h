#ifndef TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include "Type.h"
#include "Value.h"
#include "Interpreter.h"
#include <memory>
#include <optional>
#include <string>

class VarDefOrAssignment : public Instruction {
public:
    VarDefOrAssignment(const std::string &name,
        std::unique_ptr<Expression> expr,
        std::optional<Type> declatedType
        )
        : name_(name)
        , expr_(std::move(expr))
        , declaredType_(std::move(declatedType)) {}
    
    InstrResult execute([[maybe_unused]] Interpreter &interpreter) const override;
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "VarDefOrAssignment";
        return INSTR_TYPE;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
    std::unique_ptr<Expression> expr_;
    std::optional<Type> declaredType_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_VAR_DEF_H_INCLUDED
