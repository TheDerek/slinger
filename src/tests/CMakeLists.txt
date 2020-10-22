add_executable(slingertests
    regexer.t.cpp
    pathbuilder.t.cpp
)

enable_testing()
find_package(GTest REQUIRED)

target_link_libraries(slingertests PRIVATE slingerlib GTest::GTest GTest::Main)
gtest_discover_tests(slingertests)