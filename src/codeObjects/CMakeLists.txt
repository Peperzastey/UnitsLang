if(BUILD_TESTING)
    add_executable(CodeObjectsTests
        codeobjects_tests.cpp
        Instruction.cpp
        Program.cpp
        FuncDef.cpp
        InstructionBlock.cpp
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
        Program.cpp
        FuncDef.cpp
        InstructionBlock.cpp
        ../lexer/Lexer.cpp
        ../source/Source.cpp
        ../source/StringSource.cpp
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
