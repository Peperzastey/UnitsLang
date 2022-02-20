if(BUILD_TESTING)
    add_executable(LexerTests
        lexer_tests.cpp
        Lexer.cpp
        Token.cpp
        ../source/Source.cpp
        ../source/StringSource.cpp
    )

    target_link_libraries(LexerTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )

    add_test(
        NAME LexerTests
        COMMAND LexerTests
    )
endif()
