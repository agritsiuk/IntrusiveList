find_package(GTest REQUIRED)

macro(test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_compile_options(${test_name} PRIVATE -Wall -Werror -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak)
    target_link_libraries(${test_name} PRIVATE asan ubsan intrusive_list::intrusive_list GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

test(IntrusiveListTest)
