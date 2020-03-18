################################################################################
#   TWins library test application
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

cmake_minimum_required(VERSION 3.10)
project (TWins C CXX)

# Help:
# https://cmake.org/cmake/help/latest/manual/cmake-commands.7.html
# https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html

# Debug messages:
# message("Features=${CMAKE_CXX_COMPILE_FEATURES}")
# message("CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")
# message("CTEST_BINARY_DIRECTORY=${CTEST_BINARY_DIRECTORY}")
# message("PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")

# https://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# specify the C++ standard
set(CMAKE_CXX_STANDARD  14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# C++ flags
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g")

# https://medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

#
option(TWINS_BUILD_UT "Build TWins Unit Tests" ON)

if (${TWINS_BUILD_UT})
    # enable CTest
    enable_testing()
    add_subdirectory(extern/googletest)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_subdirectory(lib)

option(TWINS_BUILD_DEMO "Build TWins Demo app" OFF)

if (${TWINS_BUILD_DEMO})
    add_subdirectory(demo)
endif()
