if(BUILD_TESTING)
    add_executable(CodeObjectsTests
        codeobjects_tests.cpp
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
endif()
