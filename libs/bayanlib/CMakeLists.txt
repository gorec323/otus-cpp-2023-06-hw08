cmake_minimum_required(VERSION 3.6)

project(bayanlib  LANGUAGES CXX)

add_library(${PROJECT_NAME}
    include/scanoptions.hpp
    include/bayanscanner.hpp
    src/bayanscanner.cpp
    include/filepartitionhashcalculator.hpp
    src/filepartitionhashcalculator.cpp
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Boost::filesystem

PRIVATE
    Boost::uuid
    Boost::crc
)
