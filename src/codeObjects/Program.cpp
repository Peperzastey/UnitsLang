#include "Program.h"
#include "Interpreter.h"

int Program::execute(Interpreter &interpreter) const {
    InstrResult result = instructions_.execute(interpreter);
    switch (result) {
        case InstrResult::BREAK:
            ErrorHandler::handleJumpInstrOutsideWhile("Break not inside While");
        case InstrResult::CONTINUE:
            ErrorHandler::handleJumpInstrOutsideWhile("Continue not inside While");
        case InstrResult::RETURN: {
            std::optional<Value> retVal = interpreter.consumeReturnValue();
            if (!retVal) {
                return 0;
            }
            if (retVal->type.getTypeClass() != Type2::NUMBER || !retVal->type.asUnit().isScalar()) {
                ErrorHandler::handleTypeMismatch("Return value from main can only of scalar type");
            }
            return static_cast<int>(retVal->asDouble());
        }
        default:
            ;
    }
    return 0;
}
