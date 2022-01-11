if(BUILD_TESTING)
    add_executable(SourceTests
        source_tests.cpp
        Source.cpp
        StringSource.cpp
    )

    target_link_libraries(SourceTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )

    add_test(
        NAME SourceTests
        COMMAND SourceTests
    )
endif()
