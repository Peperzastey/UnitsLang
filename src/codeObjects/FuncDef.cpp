#include "FuncDef.h"
#include "Interpreter.h"

std::optional<Value> FuncDef::call(Interpreter &interpreter, std::vector<Value> &&args) const {
    if (args.size() != params_.size()) {
        ErrorHandler::handleFunctionCallError("Argument and parameter count mismatch for function '" + name_ + "'");
    }
    interpreter.newFuncCallContext();
    if (!std::equal(args.cbegin(), args.cend(), params_.cbegin(), [&interpreter](const Value &arg, const Variable &param) {
            if (arg.type == param.getType()) {
                interpreter.addVariable(param.getName(), std::move(arg));
                return true;
            }
            return false;
        })) {
        interpreter.deleteFuncCallContext();
        ErrorHandler::handleTypeMismatch("Argument and parameter types mismatch for function '" + name_ + "'");
    }

    InstrResult result = body_->execute(interpreter);
    std::optional<Value> retVal = interpreter.consumeReturnValue();

    switch (result) {
        case InstrResult::BREAK:
            interpreter.deleteFuncCallContext();
            ErrorHandler::handleJumpInstrOutsideWhile("Break not inside While");
        case InstrResult::CONTINUE:
            interpreter.deleteFuncCallContext();
            ErrorHandler::handleJumpInstrOutsideWhile("Continue not inside While");
        case InstrResult::NORMAL:
            if (returnType_.getTypeClass() != Type::VOID || retVal.has_value()) {
                interpreter.deleteFuncCallContext();
                ErrorHandler::handleTypeMismatch("Value returned form function '" + name_ + "' does not match its return type");
            }
            break;
        case InstrResult::RETURN:
            if (returnType_.getTypeClass() == Type::VOID && retVal.has_value()) {
                interpreter.deleteFuncCallContext();
                ErrorHandler::handleTypeMismatch("Value returned form function '" + name_ + "' does not match its return type");
            }
            if (!retVal.has_value() || returnType_ != retVal->type) {
                interpreter.deleteFuncCallContext();
                ErrorHandler::handleTypeMismatch("Value returned form function '" + name_ + "' does not match its return type");
            }
            break;
    }

    interpreter.deleteFuncCallContext();
    return retVal;
}
