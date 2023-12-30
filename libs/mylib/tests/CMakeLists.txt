cmake_minimum_required(VERSION 3.6)

project(mylib-test LANGUAGES CXX)

include(CTest)

enable_testing()

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ${GTEST_BOTH_LIBRARIES}
    mylib
)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
