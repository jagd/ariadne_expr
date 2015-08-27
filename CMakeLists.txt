project(ariadne_parser)
cmake_minimum_required(VERSION 2.8)

set(ARIADNE_SRC_PATH "ariadne")
include_directories(SYSTEM ${ARIADNE_SRC_PATH})

find_package(GTest QUIET)

if (GTEST_FOUND)
    enable_testing()
    include_directories(${GTEST_INCLUDE_DIRS})
    message("Tests are enabled")
else ()
    message("Module Google Test cannot be found\nTests are disabled")
endif ()

if ((UNIX OR CYGWIN) AND GTEST_FOUND)
    set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo Coverage CACHE TYPE INTERNAL FORCE )
    set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    find_program(GCOV gcov)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
else()
    set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE TYPE INTERNAL FORCE )
endif()

if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -march=native")
    endif(CMAKE_COMPILER_IS_GNUCC)
endif(UNIX)

add_library(parser SHARED
    src/parser.cc
    src/parser.h
    src/ast.h
    src/ast.cc
    src/interface.h
    src/interface.cc
    ${ARIADNE_SRC_PATH}/entity.cpp
    ${ARIADNE_SRC_PATH}/entity.h
    ${ARIADNE_SRC_PATH}/parameter.cpp
    ${ARIADNE_SRC_PATH}/parameter.h
    )
set_target_properties(parser PROPERTIES CXX_VISIBILITY_PRESET hidden)

add_executable(demo
	src/demo.cc
	src/interface.h
    ${ARIADNE_SRC_PATH}/entity.cpp
    ${ARIADNE_SRC_PATH}/entity.h
    ${ARIADNE_SRC_PATH}/parameter.cpp
    ${ARIADNE_SRC_PATH}/parameter.h
	)
target_link_libraries(demo parser)

########################################
if (GTEST_FOUND)
########################################

set(all_tests
    test_parser
    test_ast
    test_interface
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${all_tests})

add_custom_target(coverage
    COMMAND ${LCOV} -z -d .
    COMMAND ${CMAKE_CTEST_COMMAND}
    COMMAND ${LCOV} -d . -c -o coverage_all.txt
    COMMAND ${LCOV} -r coverage_all.txt 'test/*' '/usr/*' -o coverage_src.txt
    COMMAND ${GENHTML} --demangle-cpp -o coverage_report coverage_src.txt
    COMMAND ${CMAKE_COMMAND} -E remove coverage_all.txt coverage_src.txt
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${all_tests}
)

########################################
add_test(parser test_parser)
add_executable(test_parser
    src/parser.cc
    src/parser.h
    src/ast.h
    src/ast.cc
    t/parser.cc
)
target_link_libraries(test_parser ${GTEST_BOTH_LIBRARIES})

add_test(ast test_ast)
add_executable(test_ast
    src/ast.h
    src/ast.cc
    src/parser.h
    src/parser.cc
    t/ast.cc
)
target_link_libraries(test_ast ${GTEST_BOTH_LIBRARIES})

add_test(interface test_interface)
add_executable(test_interface
    src/ast.h
    src/ast.cc
    src/parser.h
    src/parser.cc
    src/interface.cc
    src/interface.h
    t/interface.cc
    ${ARIADNE_SRC_PATH}/entity.cpp
    ${ARIADNE_SRC_PATH}/entity.h
    ${ARIADNE_SRC_PATH}/parameter.cpp
    ${ARIADNE_SRC_PATH}/parameter.h
)
target_link_libraries(test_interface ${GTEST_BOTH_LIBRARIES})

########################################
endif (GTEST_FOUND)
########################################
