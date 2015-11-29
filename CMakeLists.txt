project(Tanuki C CXX)
cmake_minimum_required(VERSION 2.8)
include_directories(tanuki/vendor/utf8-cpp)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")


set(TESTS_SOURCES
    testing/main.cpp
    testing/framework.h
)

set(SOURCES
    tanuki/tanuki.h

    # Lexer
    tanuki/parser/parser.h
    tanuki/parser/operation.h
    tanuki/parser/tokens
    tanuki/parser/fragment.h
    tanuki/parser/rule.h
    tanuki/parser/special

    # Miscellaneous
    tanuki/misc/misc.h

    tanuki/misc/exception.h
    tanuki/misc/helper.h
    tanuki/misc/ref.h
    tanuki/misc/ref.cpp
    tanuki/misc/string
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable(TanukiTests ${TESTS_SOURCES})
SET_TARGET_PROPERTIES(TanukiTests PROPERTIES LINKER_LANGUAGE CXX)

add_library(tanuki SHARED ${SOURCES})
SET_TARGET_PROPERTIES(tanuki PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(TanukiTests tanuki)
