project(Tanuki C CXX)

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
set_target_properties(TanukiTests PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 14
)

add_library(tanuki SHARED ${SOURCES})
set_target_properties(tanuki PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 14
)

install(TARGETS tanuki DESTINATION lib)
install(DIRECTORY tanuki DESTINATION include
        FILES_MATCHING PATTERN "*.h")

target_link_libraries(TanukiTests tanuki)
