if(BUILD_TESTING)
    add_executable(SourceTests
        source_tests.cpp
        Source.cpp
    )

    target_link_libraries(SourceTests
    PRIVATE
        gtest
        gtest_main
        Threads::Threads
    )
endif()
