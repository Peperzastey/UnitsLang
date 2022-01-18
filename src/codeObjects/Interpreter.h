#ifndef TKOMSIUNITS_CODE_OBJECTS_INTERPRETER_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INTERPRETER_H_INCLUDED

#include "Program.h"
#include "Value.h"
#include "error/ErrorHandler.h"
#include <cassert>
#include <iostream>
#include <optional>
#include <stack>
#include <unordered_map>
#include <vector>

struct FuncCallContext {
    using Scope = std::unordered_map<std::string, Value>;

    void newScope() {
        scopeChain.emplace_back();
    }
    
    void deleteScope() {
        assert(!scopeChain.empty());
        scopeChain.pop_back();
    }
    
    std::optional<Value> getVariable(const std::string &name) const {
        auto riter = scopeChain.crbegin();
        for (; riter != scopeChain.crend(); ++riter) {
            if (auto varIt = riter->find(name); varIt != riter->end()) {
                return varIt->second;
            }
        }
        return std::nullopt;
    }
    
    bool addVariable(const std::string &name, Value value) {
        auto result = scopeChain.back().insert({ name, std::move(value) });
        return result.second;
    }
    
    bool assignVariable(const std::string &name, Value value) {
        auto riter = scopeChain.rbegin();
        for (; riter != scopeChain.rend(); ++riter) {
            if (auto varIt = riter->find(name); varIt != riter->end()) {
                varIt->second = std::move(value);
                return true;
            }
        }
        return false;
    }

    std::vector<Scope> scopeChain;
};

class Interpreter {
public:
    Interpreter(std::ostream &stdout, Program &programToExecute)
        : stdout_(stdout)
        , program_(programToExecute) {}
    
    int executeProgram() {
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
    
    // adds variable in current scope
    void addVariable(const std::string &name, Value value) {
        assert(!fccStack_.empty());
        if (!fccStack_.top().addVariable(name, std::move(value))) {
            ErrorHandler::handleVariableAlreadyDefined("Variable '" + name + "' already defined in current scope");
        }
    }
    
    void assignVariable(const std::string &name, Value value) {
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
    
    std::optional<Value> getVariable(const std::string &name) const {
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
    
    Value getVariableOrError(const std::string &name) const {
        std::optional<Value> val = getVariable(name);
        if (!val) {
            ErrorHandler::handleVariableNotDefined("Refernce to not-defined variable '" + name + "'");
        }
        return val.value();
    }
    
    const FuncDef* getFuncDef(const std::string &name) const {
        auto funcDef = program_.getFuncDef(name);
        if (!funcDef) {
            ErrorHandler::handleFunctionNotDefined(name);
        }
        return funcDef;
    }
    
    void setReturnValue(Value value) {
        returnValue_ = std::move(value);
    }
    
    std::optional<Value> consumeReturnValue() {
        auto temp = std::move(returnValue_);
        returnValue_.reset();
        return temp;
    }
    
    void newFuncCallContext() {
        fccStack_.emplace();
        if (!mainContext_) {
            mainContext_ = &fccStack_.top();
        } else {
            // create scope for function parameters
            fccStack_.top().newScope();
        }
    }
    
    void deleteFuncCallContext() {
        assert(!fccStack_.empty());
        fccStack_.pop();
    }
    
    void newScope() {
        assert(!fccStack_.empty());
        fccStack_.top().newScope();
    }
    
    void deleteScope() {
        assert(!fccStack_.empty());
        fccStack_.top().deleteScope();
    }
    
    const FuncCallContext::Scope& getGlobalScope() const {
        if (!mainContext_ || mainContext_->scopeChain.empty()) {
            ErrorHandler::handleFromInterpreter("Global scope was not created before referencing it");
        }
        return mainContext_->scopeChain.front();
    }
    
    FuncCallContext::Scope& getGlobalScope() {
        return const_cast<FuncCallContext::Scope &>(std::as_const(*this).getGlobalScope());
    }
    
    std::ostream& getStdout() {
        return stdout_;
    }
    
    void printLineToStdout(const std::string &text) {
        stdout_ << text << std::endl;
    }

private:
    std::ostream &stdout_;
    Program &program_;
    // explicitly use std::deque because it guarantees stable references to elements
    std::stack<FuncCallContext, std::deque<FuncCallContext>> fccStack_;
    FuncCallContext *mainContext_ = nullptr;
    // empty optional means void
    std::optional<Value> returnValue_;
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INTERPRETER_H_INCLUDED
