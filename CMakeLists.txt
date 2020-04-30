cmake_minimum_required(VERSION 3.13)
project(CompileLab)

set(CMAKE_CXX_STANDARD 14)

add_executable(clab main.cpp lexer/lexer.h parser/parser.h parser/sdtFunc.h)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})