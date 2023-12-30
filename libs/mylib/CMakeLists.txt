cmake_minimum_required(VERSION 3.6)

project(mylib  LANGUAGES CXX)

add_library(${PROJECT_NAME}
    include/mylib.hpp
    src/mylib.cpp
)

if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
