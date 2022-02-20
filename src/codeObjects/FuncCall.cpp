#include "FuncCall.h"

std::string FuncCall::getRPN() const {
    std::string output = name_ + '(';
    if (!args_.empty()) {
        output += args_.front()->getRPN();
        for (auto it = args_.cbegin() + 1; it != args_.cend(); ++it) {
            output += ',' + (*it)->getRPN();
        }  
    }
    output += ')';
    return output;
}

std::optional<Value> FuncCall::doCall(Interpreter &interpreter) const {
    const FuncDef *funcDef = interpreter.getFuncDef(name_);

    std::vector<Value> argVals;
    argVals.reserve(args_.size());
    for (auto &&arg : args_) {
        argVals.push_back(arg->calculate(interpreter));
    }
    
    return funcDef->call(interpreter, std::move(argVals));
}
