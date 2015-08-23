project(Tanuki C CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")


set(TESTS_SOURCES
    testing/main.cpp
    testing/framework.h
)

set(SOURCES
    tanuki/tanuki.h

    # Lexer
    tanuki/lexer/lexer.h
    tanuki/lexer/tokens
    tanuki/lexer/fragment.h
    tanuki/lexer/rule.h

    # Miscellaneous
    tanuki/misc/misc.h

    tanuki/misc/exception.h
    tanuki/misc/helper.h
    tanuki/misc/metaprog.h
    tanuki/misc/ref.h
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable(TanukiTests ${TESTS_SOURCES})
SET_TARGET_PROPERTIES(TanukiTests PROPERTIES LINKER_LANGUAGE CXX)

add_library(libTanuki SHARED ${SOURCES})
SET_TARGET_PROPERTIES(libTanuki PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(libTanuki re2)
target_link_libraries(TanukiTests libTanuki)

