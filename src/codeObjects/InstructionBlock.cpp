#include "InstructionBlock.h"
#include "Interpreter.h"

InstrResult InstructionBlock::execute(Interpreter &interpreter) const {
    //TODO create new scope in interpreter? - or If, While, FuncDef do it? (parameters!)
    InstrResult result = InstrResult::NORMAL;
    interpreter.newScope();
    auto instrIter = instructions_.cbegin();

    for (; instrIter != instructions_.cend() && result == InstrResult::NORMAL; ++instrIter) {
        result = (*instrIter)->execute(interpreter);
    }
    interpreter.deleteScope();
    return result;
}
