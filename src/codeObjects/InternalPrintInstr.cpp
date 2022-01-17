#include "InternalPrintInstr.h"

#include "Interpreter.h"
#include "Value.h"

InstrResult InternalPrintInstr::execute([[maybe_unused]] Interpreter &interpreter) const {
    Value stringValue = interpreter.getVariableOrError(stringVariableName_);
    interpreter.printLineToStdout(stringValue.asString());
    return InstrResult::NORMAL;
}
