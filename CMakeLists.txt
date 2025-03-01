cmake_minimum_required(VERSION 3.10)

project(intrusive_list
  LANGUAGES CXX
  DESCRIPTION "Intrusive doubly-linked list"
)

set(CMAKE_CXX_STANDARD 17)

foreach(folder in ITEMS include src tests)
    foreach(extension in ITEMS hpp cpp)
        list(APPEND GLOB_EXPRESSIONS "${folder}/*.${extension}")
    endforeach()
endforeach()

file(GLOB RECURSE ALL_SOURCES ${GLOB_EXPRESSIONS})
add_custom_target(format COMMAND clang-format -i ${ALL_SOURCES})

enable_testing()

add_library(intrusive_list INTERFACE)
add_library(intrusive_list::intrusive_list ALIAS intrusive_list)
target_include_directories(intrusive_list INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
