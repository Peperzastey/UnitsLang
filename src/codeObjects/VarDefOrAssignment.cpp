#include "VarDefOrAssignment.h"

InstrResult VarDefOrAssignment::execute([[maybe_unused]] Interpreter &interpreter) const {
    Value value = expr_->calculate(interpreter);

    if (declaredType_) {
        // this is variable definition
        if (declaredType_ != value.type) {
            ErrorHandler::handleTypeMismatch("Expression result type does not match declared variable type in variable '" + name_ + "' definition");
        }
        interpreter.addVariable(name_, std::move(value));
    } else if (auto val = interpreter.getVariable(name_); !val) {
        // this is variable definition without type declaration
        interpreter.addVariable(name_, std::move(value));
    } else {
        // this is variable assignment
        if (val->type != value.type) {
            ErrorHandler::handleTypeMismatch("Expression result type does not match variable type in variable '" + name_ + "' assignment");
        }
        interpreter.assignVariable(name_, std::move(value));
    }
    
    return InstrResult::NORMAL;
}
