# Defines the following variables:
#
# expression_parsing_static_libraries <- contains the path to expression_parsing static library and optionally: some supporting libs.
# expression_parsing_include_directories <- contains a list of directories which you should include in order to use expression_parsing.


if(EXISTS ${ExpressionParsing_DIR})
    # message("It`s OK, directory ${Pythonic_DIR} exists!")
else()
    message(FATAL_ERROR "You didn`t provide the variable with the path to the root directory of ExpressionParsing or the directory you provided as ExpressionParsing doesn`t actually exist!")
endif()


file(TO_CMAKE_PATH "${ExpressionParsing_DIR}/cmake-build-release" ExpressionParsing_release_build_dir)
file(TO_CMAKE_PATH "${ExpressionParsing_DIR}/cmake-build-debug" ExpressionParsing_debug_build_dir)


# Distinguish between build types:
if (${CMAKE_BUILD_TYPE} STREQUAL Release)
    set(ExpressionParsing_build_dir ${ExpressionParsing_release_build_dir})
elseif(${CMAKE_BUILD_TYPE} STREQUAL Debug) # It`s Debug!
    set(ExpressionParsing_build_dir ${ExpressionParsing_debug_build_dir})
else()
    message(FATAL_ERROR "Bad CMAKE_BUILD_TYPE (${CMAKE_BUILD_TYPE}). It should be either Release or Debug")
endif()


# Choose static library name based on operation system:
if(UNIX)
    set(ExpressionParsing_static_library_name libExpression_parsing_library.a)
endif()
if(WIN32)
    set(ExpressionParsing_static_library_name Expression_parsing_library.lib)
endif()

# Check if the file exists:
set(static_library_path "${ExpressionParsing_build_dir}/${ExpressionParsing_static_library_name}")

if (EXISTS ${static_library_path})
    set(expression_parsing_static_libraries ${static_library_path})
    message("Found Expression Parsing static libraries here: ${expression_parsing_static_libraries}")
else()
    message(FATAL_ERROR "Can`t find pythonic static library file in corresponding directory!!! (${static_library_path})")
endif()

# Include:
file(TO_CMAKE_PATH "${ExpressionParsing_DIR}/include" expression_parsing_include_directories)


