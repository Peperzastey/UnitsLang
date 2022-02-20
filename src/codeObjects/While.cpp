#include "While.h"

InstrResult While::execute([[maybe_unused]] Interpreter &interpreter) const {
    auto calcCond = [&interpreter, cond = cond_.get()]() {
            Value condResult = cond->calculate(interpreter);
            if (condResult.type.getTypeClass() != Type::BOOL) {
                ErrorHandler::handleTypeMismatch("Expression used as While condition must result in bool value");
            }
            return condResult.asBool();
        };
    auto notBreakOrReturn = [](InstrResult result) {
            switch (result) {
                case InstrResult::BREAK:
                case InstrResult::RETURN:
                    return false;
                default:
                    return true;
            }
        };
    InstrResult result = InstrResult::NORMAL;

    for (bool cond = calcCond(); cond && notBreakOrReturn(result); cond = calcCond()) {
        result = body_->execute(interpreter);
    }
    
    if (result == InstrResult::BREAK || result == InstrResult::CONTINUE) {
        result = InstrResult::NORMAL;
    }
    return result;
}
