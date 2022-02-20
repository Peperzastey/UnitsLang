#include "Program.h"
#include "Interpreter.h"

Program::Program(
        std::vector<std::unique_ptr<FuncDef>> &&funcDefs,
        std::vector<std::unique_ptr<Instruction>> &&instructions
    )
    : instructions_(std::move(instructions)) {
    addPredefinedPrintFunc();
    for (auto &&func : funcDefs) {
        addFuncDef(std::move(func));
    }
}

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
            if (retVal->type.getTypeClass() != Type::NUMBER || !retVal->type.asUnit().isScalar()) {
                ErrorHandler::handleTypeMismatch("Return value from main can only of scalar type");
            }
            return static_cast<int>(retVal->asDouble());
        }
        default:
            ;
    }
    return 0;
}

const FuncDef* Program::getFuncDef(const std::string &name) const {
    auto iter = funcDefs_.find(name);
    return (iter != funcDefs_.cend() ? iter->second.get() : nullptr);
}

void Program::addFuncDef(std::unique_ptr<FuncDef> funcDef) {
    std::string name = funcDef->getName();
    auto [_, success] = funcDefs_.insert({ name, std::move(funcDef) });
    (void)_;
    if (!success) {
        std::ostringstream os;
        os << "Redefinition of function named '" << name << "'";
        ErrorHandler::handleFromCodeObject(os.str());
    }
}

void Program::addPredefinedPrintFunc() {
    std::vector<std::unique_ptr<Instruction>> printInstructions;
    printInstructions.emplace_back(std::make_unique<InternalPrintInstr>("_"));
    auto printFuncInstrBlock = std::make_unique<InstructionBlock>(
            std::move(printInstructions)
        );
    auto printFuncDef = std::make_unique<FuncDef>(
            "print",
            std::vector{ Variable{"_", Type::STRING} },
            Type::VOID,
            std::move(printFuncInstrBlock)
        );
    addFuncDef(std::move(printFuncDef));
}
