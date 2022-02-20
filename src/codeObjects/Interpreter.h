#ifndef TKOMSIUNITS_CODE_OBJECTS_INTERPRETER_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INTERPRETER_H_INCLUDED

#include "Value.h"
#include "error/ErrorHandler.h"
#include <cassert>
#include <iostream>
#include <optional>
#include <stack>
#include <unordered_map>
#include <vector>

class Program;
class FuncDef;

struct FuncCallContext {
    using Scope = std::unordered_map<std::string, Value>;

    void newScope();
    void deleteScope();
    
    std::optional<Value> getVariable(const std::string &name) const;
    bool addVariable(const std::string &name, Value value);
    bool assignVariable(const std::string &name, Value value);

public:
    std::vector<Scope> scopeChain;
};

class Interpreter {
public:
    Interpreter(std::ostream &stdout, Program &programToExecute)
        : stdout_(stdout)
        , program_(programToExecute) {}
    
    int executeProgram();
    
    // adds variable in current scope
    void addVariable(const std::string &name, Value value);
    void assignVariable(const std::string &name, Value value);
    std::optional<Value> getVariable(const std::string &name) const;
    Value getVariableOrError(const std::string &name) const;
    
    const FuncDef* getFuncDef(const std::string &name) const;
    
    void setReturnValue(Value value);
    std::optional<Value> consumeReturnValue();
    
    void newFuncCallContext();
    void deleteFuncCallContext();
    
    void newScope();
    void deleteScope();
    
    const FuncCallContext::Scope& getGlobalScope() const;
    FuncCallContext::Scope& getGlobalScope();
    
    std::ostream& getStdout();
    void printLineToStdout(const std::string &text);

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
