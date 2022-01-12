#ifndef TKOMSIUNITS_CODE_OBJECTS_INSTR_RESULT_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_INSTR_RESULT_H_INCLUDED

enum class InstrResult {
    NORMAL,
    RETURN, // +value -- set in stack in ExecutionContext(/Interpreter)?
    BREAK,
    CONTINUE
    //EXCEPTION ? // +exception type
};

#endif // TKOMSIUNITS_CODE_OBJECTS_INSTR_RESULT_H_INCLUDED
