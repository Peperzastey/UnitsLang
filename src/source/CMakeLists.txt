#TODO build library for source module

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

    #TODO
    #add_test( #TODO gtest module's add_test
    #    NAME MinWindowSubstrTests
    #    COMMAND minWindowSubstrTests
    #)
endif()
