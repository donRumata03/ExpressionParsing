cmake_minimum_required(VERSION 3.16)
project(Parsing_expressions)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

# find_package(OpenCV QUIET)

add_library(Expression_parsing_library
        include/expression_parsing.h
        include/expression_particle.h
        include/expression_segment.h
        include/expr_token.h
        include/dictionary.h
        include/final_token.h
        include/parse_utils.h
        include/scenery.h
        include/tokenizer.h
        include/validator.h

        src/dictionary.cpp
        src/expr_token.cpp
        src/expression_particle.cpp
        src/final_token.cpp
        src/validation.cpp
        src/parse_utils.cpp

        src/expression_tree.cpp
        include/expression_tree.h
        include/flat_expression_tree.h
        src/flat_expression_tree.cpp
        src/tree_tokenizer.cpp
        include/tree_tokenizer.h
        include/_letter_utils.h

        )

add_executable(Parsing_expressions

        test/main.cpp

        test/benchmark.h

        test/Timer.h test/Timer.cpp test/tests.h test/encoding_test.h test/encoder.h test/tree_tests.h test/coloured_printer.h test/_print_stl.h)

target_link_libraries(Parsing_expressions Expression_parsing_library)

if (DCMAKE_BUILD_TYPE STREQUAL Release)
    message(It`s_Release_mode!)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    message(It`s_Debug_mode!)
endif()
