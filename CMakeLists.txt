cmake_minimum_required(VERSION 3.6)

set(PATCH_VERSION 1 CACHE INTERNAL "Patch version. Default = 1")
set(PROJECT_VERSION 0.0.${PATCH_VERSION} CACHE INTERNAL "Application version" FORCE)

project(bayan VERSION ${PROJECT_VERSION} LANGUAGES CXX)

configure_file(
    version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES filesystem system uuid crc program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG ad09f66 # tag boost-1.84.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(boost)


if (BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

add_subdirectory(libs)

add_executable(${PROJECT_NAME}
    main.cpp
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    Boost::program_options
    bayanlib
)

if (UNIX)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "bayan application")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "My Corporation")
    include(CPack)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen.conf.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/README.md\ ${CMAKE_CURRENT_SOURCE_DIR}/ )

    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docs/)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})


    add_custom_target(doxygen COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        COMMENT "Generating docs"
        VERBATIM
    )
else(DOXYGEN_FOUND)
    message(STATUS "Doxygen not found - reference manual will not be created")
endif(DOXYGEN_FOUND)

