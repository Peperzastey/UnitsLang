#include "If.h"

#include "Value.h"

InstrResult If::execute([[maybe_unused]] Interpreter &interpreter) const {
    if (cond_) {
        Value condResult = cond_->calculate(interpreter);
        if (condResult.type.getTypeClass() != Type::BOOL) {
            ErrorHandler::handleTypeMismatch("Expression used as If condition must result in bool value");
        }
        if (condResult.asBool()) {
            // True
            return positiveBlock_->execute(interpreter);
        } else if (elseIf_) {
            // False
            return elseIf_->execute(interpreter);
        }
        return InstrResult::NORMAL;
    } else {
        // Else
        return positiveBlock_->execute(interpreter);
    }
}
