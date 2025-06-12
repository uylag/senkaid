add_executable(tests test_main.cpp)
target_link_libraries(tests PRIVATE senkaid)
add_test(NAME senkaid_tests COMMAND tests)
