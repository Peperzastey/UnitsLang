if(BUILD_TESTING)
    add_executable(ParserTests
        parser_tests.cpp
        ../codeObjects/Instruction.cpp
        ../lexer/Lexer.cpp
        ../source/Source.cpp
        ../source/StringSource.cpp
    )

    target_link_libraries(ParserTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )
endif()
