cmake_minimum_required(VERSION 3.13)

project(SymulacjaSieci)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors)

include_directories(
        include
        mocks
)

file(GLOB SOURCE_FILES
        src/*.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} main.cpp)

enable_testing()
include(test/CMakeLists.txt)
