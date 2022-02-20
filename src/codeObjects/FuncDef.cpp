#include "FuncDef.h"
#include "Interpreter.h"

FuncDef::FuncDef(
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

std::string FuncDef::toString() const {
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
