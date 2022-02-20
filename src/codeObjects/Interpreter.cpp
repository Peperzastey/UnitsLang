#include "Interpreter.h"

#include "Program.h"

int Interpreter::executeProgram() {
    newFuncCallContext();
    int exitStatus = 0;
    try {
        exitStatus = program_.execute(*this);
    } catch (const std::exception &e) {
        deleteFuncCallContext();
        std::cerr << e.what() << std::endl;
        return 1; // failure
    }
    deleteFuncCallContext();
    return exitStatus;
}

void Interpreter::addVariable(const std::string &name, Value value) {
    assert(!fccStack_.empty());
    if (!fccStack_.top().addVariable(name, std::move(value))) {
        ErrorHandler::handleVariableAlreadyDefined("Variable '" + name + "' already defined in current scope");
    }
}

void Interpreter::assignVariable(const std::string &name, Value value) {
    assert(!fccStack_.empty());
    if (fccStack_.top().assignVariable(name, std::move(value))) {
        return;
    }
    if (mainContext_ != &fccStack_.top()) {
        // search in global scope
        if (auto iter = getGlobalScope().find(name); iter != getGlobalScope().end()) {
            iter->second = std::move(value);
            return;
        }
    }
    ErrorHandler::handleVariableNotDefined("Refernce to not-defined variable '" + name + "'");
}
    
std::optional<Value> Interpreter::getVariable(const std::string &name) const {
    assert(!fccStack_.empty());
    std::optional<Value> val = fccStack_.top().getVariable(name);
    if (val) {
        return val;
    }
    if (mainContext_ != &fccStack_.top()) {
        // search in global scope
        if (auto iter = getGlobalScope().find(name); iter != getGlobalScope().end()) {
            return iter->second;
        }
    }
    return std::nullopt;
}
    
Value Interpreter::getVariableOrError(const std::string &name) const {
    std::optional<Value> val = getVariable(name);
    if (!val) {
        ErrorHandler::handleVariableNotDefined("Refernce to not-defined variable '" + name + "'");
    }
    return val.value();
}

const FuncDef* Interpreter::getFuncDef(const std::string &name) const {
    auto funcDef = program_.getFuncDef(name);
    if (!funcDef) {
        ErrorHandler::handleFunctionNotDefined(name);
    }
    return funcDef;
}

void Interpreter::setReturnValue(Value value) {
    returnValue_ = std::move(value);
}

std::optional<Value> Interpreter::consumeReturnValue() {
    auto temp = std::move(returnValue_);
    returnValue_.reset();
    return temp;
}

void Interpreter::newFuncCallContext() {
    fccStack_.emplace();
    if (!mainContext_) {
        mainContext_ = &fccStack_.top();
    } else {
        // create scope for function parameters
        fccStack_.top().newScope();
    }
}

void Interpreter::deleteFuncCallContext() {
    assert(!fccStack_.empty());
    fccStack_.pop();
}

void Interpreter::newScope() {
    assert(!fccStack_.empty());
    fccStack_.top().newScope();
}

void Interpreter::deleteScope() {
    assert(!fccStack_.empty());
    fccStack_.top().deleteScope();
}

const FuncCallContext::Scope& Interpreter::getGlobalScope() const {
    if (!mainContext_ || mainContext_->scopeChain.empty()) {
        ErrorHandler::handleFromInterpreter("Global scope was not created before referencing it");
    }
    return mainContext_->scopeChain.front();
}

FuncCallContext::Scope& Interpreter::getGlobalScope() {
    return const_cast<FuncCallContext::Scope &>(std::as_const(*this).getGlobalScope());
}

std::ostream& Interpreter::getStdout() {
    return stdout_;
}

void Interpreter::printLineToStdout(const std::string &text) {
    stdout_ << text << std::endl;
}

void FuncCallContext::newScope() {
    scopeChain.emplace_back();
}

void FuncCallContext::deleteScope() {
    assert(!scopeChain.empty());
    scopeChain.pop_back();
}

std::optional<Value> FuncCallContext::getVariable(const std::string &name) const {
    auto riter = scopeChain.crbegin();
    for (; riter != scopeChain.crend(); ++riter) {
        if (auto varIt = riter->find(name); varIt != riter->end()) {
            return varIt->second;
        }
    }
    return std::nullopt;
}

bool FuncCallContext::addVariable(const std::string &name, Value value) {
    auto result = scopeChain.back().insert({ name, std::move(value) });
    return result.second;
}

bool FuncCallContext::assignVariable(const std::string &name, Value value) {
    auto riter = scopeChain.rbegin();
    for (; riter != scopeChain.rend(); ++riter) {
        if (auto varIt = riter->find(name); varIt != riter->end()) {
            varIt->second = std::move(value);
            return true;
        }
    }
    return false;
}
