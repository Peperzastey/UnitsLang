if(BUILD_TESTING)
    add_executable(ParserTests
        parser_tests.cpp
        Parser.cpp
        ../codeObjects/Instruction.cpp
        ../codeObjects/InstructionBlock.cpp
        ../codeObjects/FuncDef.cpp
        ../codeObjects/BinaryExpression.cpp
        ../codeObjects/FuncCall.cpp
        ../codeObjects/If.cpp
        ../codeObjects/Interpreter.cpp
        ../codeObjects/Program.cpp
        ../codeObjects/Unit.cpp
        ../codeObjects/VarDefOrAssignment.cpp
        ../codeObjects/While.cpp
        ../codeObjects/InternalPrintInstr.cpp
        ../lexer/Lexer.cpp
        ../lexer/Token.cpp
        ../source/Source.cpp
        ../source/StringSource.cpp
    )

    target_link_libraries(ParserTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )
    
    add_test(
        NAME ParserTests
        COMMAND ParserTests
    )
endif()
