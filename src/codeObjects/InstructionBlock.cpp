#include "InstructionBlock.h"
#include "Interpreter.h"

InstrResult InstructionBlock::execute(Interpreter &interpreter) const {
    InstrResult result = InstrResult::NORMAL;
    interpreter.newScope();
    auto instrIter = instructions_.cbegin();

    for (; instrIter != instructions_.cend() && result == InstrResult::NORMAL; ++instrIter) {
        result = (*instrIter)->execute(interpreter);
    }
    interpreter.deleteScope();
    return result;
}
