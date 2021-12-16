#ifndef TKOMSIUNITS_CODE_OBJECTS_RETURN_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_RETURN_H_INCLUDED

#include "Instruction.h"
#include "Expression.h"
#include <memory>
#include <string>

class Return : public Instruction {
public:
    Return(std::unique_ptr<Expression> expr)
        : expr_(std::move(expr)) {}
    
    const std::string& getInstrType() const {
        static const std::string INSTR_TYPE = "Return";
        return INSTR_TYPE;
    }
    
    void execute() {
        if (expr_) {
            //expr_->execute();
        } else {
            //... (return;)
        }
    }
    
private:
    std::unique_ptr<Expression> expr_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_RETURN_H_INCLUDED
