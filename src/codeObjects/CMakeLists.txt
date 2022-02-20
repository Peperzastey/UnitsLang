if(BUILD_TESTING)
    add_executable(CodeObjectsTests
        codeobjects_tests.cpp
        Instruction.cpp
        BinaryExpression.cpp
        Program.cpp
        FuncDef.cpp
        InstructionBlock.cpp
        InternalPrintInstr.cpp
        FuncCall.cpp
        If.cpp
        Interpreter.cpp
        Unit.cpp
        VarDefOrAssignment.cpp
        While.cpp
        ../lexer/Token.cpp
    )

    target_link_libraries(CodeObjectsTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )
    
    add_test(
        NAME CodeObjectsTests
        COMMAND CodeObjectsTests
    )
    
    add_executable(InterpreterTests
        interpreter_tests.cpp
        Instruction.cpp
        BinaryExpression.cpp
        Program.cpp
        FuncDef.cpp
        InstructionBlock.cpp
        InternalPrintInstr.cpp
        ../parser/Parser.cpp
        ../lexer/Lexer.cpp
        ../lexer/Token.cpp
        ../source/Source.cpp
        ../source/StringSource.cpp
        FuncCall.cpp
        If.cpp
        Interpreter.cpp
        Unit.cpp
        VarDefOrAssignment.cpp
        While.cpp
    )

    target_link_libraries(InterpreterTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )
    
    add_test(
        NAME InterpreterTests
        COMMAND InterpreterTests
    )
endif()
